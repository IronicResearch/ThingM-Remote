/*
    File:		SerialPortSample.c
	
    Description:	This sample demonstrates how to use IOKitLib to find all serial ports on the system.
                        It also shows how to open, write to, read from, and close a serial port.
                
    Copyright:		© Copyright 2000 Apple Computer, Inc. All rights reserved.
	
    Disclaimer:		IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
                        ("Apple") in consideration of your agreement to the following terms, and your
                        use, installation, modification or redistribution of this Apple software
                        constitutes acceptance of these terms.  If you do not agree with these terms,
                        please do not use, install, modify or redistribute this Apple software.

                        In consideration of your agreement to abide by the following terms, and subject
                        to these terms, Apple grants you a personal, non-exclusive license, under Apple’s
                        copyrights in this original Apple software (the "Apple Software"), to use,
                        reproduce, modify and redistribute the Apple Software, with or without
                        modifications, in source and/or binary forms; provided that if you redistribute
                        the Apple Software in its entirety and without modifications, you must retain
                        this notice and the following text and disclaimers in all such redistributions of
                        the Apple Software.  Neither the name, trademarks, service marks or logos of
                        Apple Computer, Inc. may be used to endorse or promote products derived from the
                        Apple Software without specific prior written permission from Apple.  Except as
                        expressly stated in this notice, no other rights or licenses, express or implied,
                        are granted by Apple herein, including but not limited to any patent rights that
                        may be infringed by your derivative works or by other works in which the Apple
                        Software may be incorporated.

                        The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
                        WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
                        WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
                        PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
                        COMBINATION WITH YOUR PRODUCTS.

                        IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
                        CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
                        GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
                        ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
                        OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
                        (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
                        ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
				
	Change History (most recent first):
        
            <4>		06/27/03	Modified for any/all serial ports. (DaveM)
            <3>		12/19/00	New IOKit keys.
            <2>	 	08/22/00	Incorporated changes from code review.
            <1>	 	08/03/00	New sample.
                    
        To compile for standard console output at user terminal: (DaveM)
        
        gcc SerialPortSample.c -o SerialPortSample.out -framework IOKit -framework CoreFoundation
        
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <paths.h>
#include <termios.h>
#include <sysexits.h>
#include <sys/param.h>

#include <CoreFoundation/CoreFoundation.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>

#define kATCommandString	"P5\r\0\r" /* "AT\r" */
#define kOKResponseString	"OK"

enum {
    kNumRetries = 3
};

// Hold the original termios attributes so we can reset them
static struct termios gOriginalTTYAttrs;

static kern_return_t FindSerialDevices(io_iterator_t *matchingServices);
static kern_return_t GetSerialDevicePath(io_iterator_t serialPortIterator, char *bsdPath, CFIndex maxPathSize, char* oldName);
static int OpenSerialPort(const char *bsdPath);
static Boolean InitializeSerialDevice(int fileDescriptor);
static void CloseSerialPort(int fileDescriptor);

// Returns an iterator across all known serial devices. Caller is responsible for
// releasing the iterator when iteration is complete.
static kern_return_t FindSerialDevices(io_iterator_t *matchingServices)
{
    kern_return_t		kernResult; 
    mach_port_t			masterPort;
    CFMutableDictionaryRef	classesToMatch;

/*! @function IOMasterPort
    @abstract Returns the mach port used to initiate communication with IOKit.
    @discussion Functions that don't specify an existing object require the IOKit master port to be passed. This function obtains that port.
    @param bootstrapPort Pass MACH_PORT_NULL for the default.
    @param masterPort The master port is returned.
    @result A kern_return_t error code. */

    kernResult = IOMasterPort(MACH_PORT_NULL, &masterPort);
    if (KERN_SUCCESS != kernResult)
        printf("IOMasterPort returned %d\n", kernResult);
        
/*! @function IOServiceMatching
    @abstract Create a matching dictionary that specifies an IOService class match.
    @discussion A very common matching criteria for IOService is based on its class. IOServiceMatching will create a matching dictionary that specifies any IOService of a class, or its subclasses. The class is specified by C-string name.
    @param name The class name, as a const C-string. Class matching is successful on IOService's of this class or any subclass.
    @result The matching dictionary created, is returned on success, or zero on failure. The dictionary is commonly passed to IOServiceGetMatchingServices or IOServiceAddNotification which will consume a reference, otherwise it should be released with CFRelease by the caller. */

    // Serial devices are instances of class IOSerialBSDClient
    classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
    if (classesToMatch == NULL)
        printf("IOServiceMatching returned a NULL dictionary.\n");
    else {
/*!
	@function CFDictionarySetValue
	Sets the value of the key in the dictionary.
	@param theDict The dictionary to which the value is to be set. If this
		parameter is not a valid mutable CFDictionary, the behavior is
		undefined. If the dictionary is a fixed-capacity dictionary and
		it is full before this operation, and the key does not exist in
		the dictionary, the behavior is undefined.
	@param key The key of the value to set into the dictionary. If a key 
		which matches this key is already present in the dictionary, only
		the value is changed ("add if absent, replace if present"). If
		no key matches the given key, the key-value pair is added to the
		dictionary. If added, the key is retained by the dictionary,
		using the retain callback provided
		when the dictionary was created. If the key is not of the sort
		expected by the key retain callback, the behavior is undefined.
	@param value The value to add to or replace into the dictionary. The value
		is retained by the dictionary using the retain callback provided
		when the dictionary was created, and the previous value if any is
		released. If the value is not of the sort expected by the
		retain or release callbacks, the behavior is undefined.
*/
        CFDictionarySetValue(classesToMatch,
                            CFSTR(kIOSerialBSDTypeKey),
                            CFSTR(kIOSerialBSDRS232Type));
        // Each serial device object has a property with key
        // kIOSerialBSDTypeKey and a value that is one of kIOSerialBSDAllTypes,
        // kIOSerialBSDModemType, or kIOSerialBSDRS232Type. You can experiment with the
        // matching by changing the last parameter in the above call to CFDictionarySetValue.
        
        // As shipped, this sample is only interested in modems,
        // so add this property to the CFDictionary we're matching on. 
        // This will find devices that advertise themselves as modems,
        // such as built-in and USB modems. However, this match won't find serial modems.
    }
    
    /*! @function IOServiceGetMatchingServices
        @abstract Look up registered IOService objects that match a matching dictionary.
        @discussion This is the preferred method of finding IOService objects currently registered by IOKit. IOServiceAddNotification can also supply this information and install a notification of new IOServices. The matching information used in the matching dictionary may vary depending on the class of service being looked up.
        @param masterPort The master port obtained from IOMasterPort().
        @param matching A CF dictionary containing matching information, of which one reference is consumed by this function. IOKitLib can contruct matching dictionaries for common criteria with helper functions such as IOServiceMatching, IOOpenFirmwarePathMatching.
        @param existing An iterator handle is returned on success, and should be released by the caller when the iteration is finished.
        @result A kern_return_t error code. */

    kernResult = IOServiceGetMatchingServices(masterPort, classesToMatch, matchingServices);    
    if (KERN_SUCCESS != kernResult)
        printf("IOServiceGetMatchingServices returned %d\n", kernResult);
        
    return kernResult;
}
    
// Given an iterator across a set of serial devices, return the BSD path to the first one.
// If no serial devices are found the path name is set to an empty string.
static kern_return_t GetSerialDevicePath(io_iterator_t serialPortIterator, char *bsdPath, CFIndex maxPathSize, char *oldName)
{
    io_object_t		modemService;
    kern_return_t	kernResult = KERN_FAILURE;
    Boolean		modemFound = false;
    int			deviceCount = 0;
    
    // Initialize the returned path
    *bsdPath = '\0';
    
/*! @function IOIteratorNext
    @abstract Returns the next object in an iteration.
    @discussion This function returns the next object in an iteration, or zero if no more remain or the iterator is invalid.
    @param iterator An IOKit iterator handle.
    @result If the iterator handle is valid, the next element in the iteration is returned, otherwise zero is returned. The element should be released by the caller when it is finished. */

    while ((modemService = IOIteratorNext(serialPortIterator)) && !modemFound )
    {
        CFTypeRef	modemNameAsCFString;
        CFTypeRef	bsdPathAsCFString;
        

/*! @function IORegistryEntryCreateCFProperty
    @abstract Create a CF representation of a registry entry's property.
    @discussion This function creates an instantaneous snapshot of a registry entry property, creating a CF container analogue in the caller's task. Not every object available in the kernel is represented as a CF container; currently OSDictionary, OSArray, OSSet, OSSymbol, OSString, OSData, OSNumber, OSBoolean are created as their CF counterparts. 
    @param entry The registry entry handle whose property to copy.
    @param key A CFString specifying the property name.
    @param allocator The CF allocator to use when creating the CF container.
    @param options No options are currently defined.
    @result A CF container is created and returned the caller on success. The caller should release with CFRelease. */

        modemNameAsCFString = IORegistryEntryCreateCFProperty(modemService,
                                                              CFSTR(kIOTTYDeviceKey),
                                                              kCFAllocatorDefault,
                                                              0);
        if (modemNameAsCFString)
        {
            char modemName[128];
            Boolean result;
            
            result = CFStringGetCString(modemNameAsCFString,
                                        modemName,
                                        sizeof(modemName), 
                                        kCFStringEncodingASCII);
            CFRelease(modemNameAsCFString);
            
            if (result)
            {
                printf("Serial stream name: %s, ", modemName);
                modemFound = true;
                deviceCount++;
                kernResult = KERN_SUCCESS;
            }
        }

        bsdPathAsCFString = IORegistryEntryCreateCFProperty(modemService,
                                                            CFSTR(kIOCalloutDeviceKey),
                                                            kCFAllocatorDefault,
                                                            0);
        if (bsdPathAsCFString)
        {
            Boolean result;
            
            result = CFStringGetCString(bsdPathAsCFString,
                                        bsdPath,
                                        maxPathSize, 
                                        kCFStringEncodingASCII);
            CFRelease(bsdPathAsCFString);
            
            if (result)
                printf("BSD path: %s", bsdPath);
        }

        printf("\n");

/*! @function IOObjectRelease
    @abstract Releases an object handle previously returned by IOKitLib.
    @discussion All objects returned by IOKitLib should be released with this function when access to them is no longer needed. Using the object after it has been released may or may not return an error, depending on how many references the task has to the same object in the kernel.
    @param object The IOKit object to release.
    @result A kern_return_t error code. */
    
        (void) IOObjectRelease(modemService);
        // We have sucked this service dry of information so release it now.
        
        // If serial device was found, try matching up with old port name.
        if (modemFound) {
            if (strlen(oldName) > 2) {
                // If "\p.Aout" name does not match device count, keep looking.
                if (oldName[2] != 'A'+deviceCount-1)
                    modemFound = false;
                }
            }
    }
        
    return kernResult;
}

// Given the path to a serial device, open the device and configure it.
// Return the file descriptor associated with the device.
static int OpenSerialPort(const char *bsdPath)
{
    int 		fileDescriptor = -1;
    struct termios	options;
    
    fileDescriptor = open(bsdPath, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fileDescriptor == -1)
    {
        printf("Error opening serial port %s - %s(%d).\n",
               bsdPath, strerror(errno), errno);
        goto error;
    }

    if (fcntl(fileDescriptor, F_SETFL, 0) == -1)
    {
        printf("Error clearing O_NDELAY %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
        goto error;
    }
    
    // Get the current options and save them for later reset
    if (tcgetattr(fileDescriptor, &gOriginalTTYAttrs) == -1)
    {
        printf("Error getting tty attributes %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
        goto error;
    }

    // Set raw input, one second timeout
    // These options are documented in the man page for termios
    // (in Terminal enter: man termios)
    options = gOriginalTTYAttrs;
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    options.c_cc[ VMIN ] = 0;
    options.c_cc[ VTIME ] = 10;
    
    // Set the options
    if (tcsetattr(fileDescriptor, TCSANOW, &options) == -1)
    {
        printf("Error setting tty attributes %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
        goto error;
    }

    // Success
    return fileDescriptor;
    
    // Failure path
error:
    if (fileDescriptor != -1)
        close(fileDescriptor);
    return -1;
}

// Given the file descriptor for a modem device, attempt to initialize the modem by sending it
// a standard AT command and reading the response. If successful the response is "OK".
// Return true if successful, otherwise false.
static Boolean InitializeSerialDevice(int fileDescriptor)
{
    char	buffer[ 255 ];	// Input buffer
    char	*bufPtr;	// Current char in buffer
    ssize_t	numBytes;	// Number of bytes read or written
    int		tries;		// Number of tries so far
    
    for (tries = 0; tries < kNumRetries; tries++)
    {
        // Send an AT command to the modem
        numBytes = write(fileDescriptor, kATCommandString, strlen(kATCommandString));
        if (numBytes == -1)
        {
            printf("Error writing to serial device - %s(%d).\n",
                   strerror(errno), errno);
            continue;
        }
        else if (numBytes < strlen(kATCommandString)) 
            continue;
            
        // Read characters into our buffer until we get a CR or LF
        bufPtr = buffer;
        do
        {
            numBytes = read(fileDescriptor, bufPtr, &buffer[sizeof(buffer)] - bufPtr - 1);
            if (numBytes == -1)
            {
                printf("Error reading from serial device - %s(%d).\n",
                    strerror(errno), errno);
            }
            else if (numBytes > 0)
            {
                bufPtr += numBytes;
                if (*(bufPtr - 1) == '\n' || *(bufPtr - 1) == '\r')
                    break;
            }
        } while (numBytes > 0);
        
        // NUL terminate the string and see if we got an OK response
        *bufPtr = '\0';
        
        // See what we got (DaveM)
        printf("Serial Response: %s\n", buffer);
        
        if (strncmp(buffer, kOKResponseString, strlen(kOKResponseString)) == 0)
            return true;
    }
    
    return false;
}

// Return true if successful, otherwise false.
static Boolean OutputSerialDevice(int fileDescriptor, char* outbuf)
{
    char	buffer[ 255 ];	// Input buffer
    char	*bufPtr;	// Current char in buffer
    ssize_t	numBytes;	// Number of bytes read or written
    
        // Send output buffer string to serial device.
        strcpy(buffer, outbuf);
        numBytes = write(fileDescriptor, buffer, strlen(buffer));
        if (numBytes == -1)
        {
            printf("Error writing to serial device - %s(%d).\n",
                   strerror(errno), errno);
            return false;
        }
        else if (numBytes < strlen(buffer)) 
            return false;
                
        return true;
}
 
// Return true if successful, otherwise false.
static Boolean InputSerialDevice(int fileDescriptor, char* inbuf)
{
    char	buffer[ 255 ];	// Input buffer
    char	*bufPtr;	// Current char in buffer
    ssize_t	numBytes;	// Number of bytes read or written

        // Read characters into our buffer until we get a CR or LF
        bufPtr = buffer;
        do
        {
            numBytes = read(fileDescriptor, bufPtr, &buffer[sizeof(buffer)] - bufPtr - 1);
            if (numBytes == -1)
            {
                printf("Error reading from serial device - %s(%d).\n",
                    strerror(errno), errno);
            }
            else if (numBytes > 0)
            {
                bufPtr += numBytes;
                if (*(bufPtr - 1) == '\n' || *(bufPtr - 1) == '\r')
                    break;
            }
            else if (numBytes == 0)
            {
                return false;
            }

        } while (numBytes > 0);
        
        // NUL terminate the string and see if we got an OK response
        *bufPtr = '\0';
        
        // Copy to input buffer
        strcpy(inbuf, buffer);
            
    return true;
}

// Given the file descriptor for a serial device, close that device.
void CloseSerialPort(int fileDescriptor)
{
    // Traditionally it is good to reset a serial port back to
    // the state in which you found it.  Let's continue that tradition.
    if (tcsetattr(fileDescriptor, TCSANOW, &gOriginalTTYAttrs) == -1)
    {
        printf("Error resetting tty attributes - %s(%d).\n",
            strerror(errno), errno);
    }

    close(fileDescriptor);
}

/* Serial IO routines based on SerialTerm.c terminal dialog. */

    int			fileDescriptor;

int InitSio(char* portname)
{
    kern_return_t	kernResult; // on PowerPC this is an int (4 bytes)
/*
 *	error number layout as follows (see mach/error.h):
 *
 *	hi		 		       lo
 *	| system(6) | subsystem(12) | code(14) |
 */

    io_iterator_t	serialPortIterator;
    char		bsdPath[ MAXPATHLEN ];
     
    // Modified to search for all serial devices, and use the last one found. (DaveM)
    printf("Searching for all Serial Devices...\n");
    kernResult = FindSerialDevices(&serialPortIterator);
    kernResult = GetSerialDevicePath(serialPortIterator, bsdPath, sizeof(bsdPath), portname);
    IOObjectRelease(serialPortIterator);	// Release the iterator.

    // Now open the modem port we found, initialize the modem then close it
    if (!bsdPath[0]) {
        printf("No serial port found.\n");
        return EX_UNAVAILABLE;
    }

    fileDescriptor = OpenSerialPort(bsdPath);
    if (-1 == fileDescriptor) {
        printf("Serial Device %s could not be opened.\n", bsdPath);
        return EX_IOERR;
    }
    printf("Serial Device %s opened.\n", bsdPath);
/*    
    if (InitializeSerialDevice(fileDescriptor))
        printf("Serial Device %s initialized.\n", bsdPath);
    else
        printf("Serial Device %s could not be initialized.\n", bsdPath);
*/        
    return EX_OK;
}

int OutSio(char* outbuf)
{
    return (OutputSerialDevice(fileDescriptor, outbuf));
}

int InSio(char* inbuf)
{
    return (InputSerialDevice(fileDescriptor, inbuf));
}

int ExitSio(void)
{        
    CloseSerialPort(fileDescriptor);
    printf("Serial port closed.\n");
        
    return EX_OK;
}
