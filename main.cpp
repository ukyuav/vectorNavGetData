#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <windows.h> 

// Include this header file to get access to VectorNav sensors.
#include "vn/sensors.h"
// We need this file for our sleep function.
#include "vn/thread.h"

#define PACKETSIZE 110

using namespace std;
using namespace vn::math;
using namespace vn::sensors;
using namespace vn::protocol::uart;
using namespace vn::xplat;
// Method declarations for future use.
void asciiOrBinaryAsyncMessageReceived(void* userData, Packet& p, size_t index);

//base filename
char outFileStr[] = "VECTORNAVDATA00.CSV";
ofstream outFile;


int main(int argc, char *argv[])
{
	// This example walks through using the VectorNav C++ Library to connect to
	// and interact with a VectorNav sensor.
	// First determine which COM port your sensor is attached to and update the
	// constant below. Also, if you have changed your sensor from the factory
	// default baudrate of 115200, you will need to update the baudrate
	// constant below as well.
	const string SensorPort = argv[1];                             // Windows format for physical and virtual (USB) serial port.
																  // const string SensorPort = "/dev/ttyS1";                    // Linux format for physical serial port.
																  // const string SensorPort = "/dev/ttyUSB0";                  // Linux format for virtual (USB) serial port.
																  // const string SensorPort = "/dev/tty.usbserial-FTXXXXXX";   // Mac OS X format for virtual (USB) serial port.
																  // const string SensorPort = "/dev/ttyS0";                    // CYGWIN format. Usually the Windows COM port number minus 1. This would connect to COM1.
	const uint32_t SensorBaudrate = 230400;
	// Now let's create a VnSensor object and use it to connect to our sensor.
	VnSensor vs;
	vs.connect(SensorPort, SensorBaudrate);
	// Let's query the sensor's model number.
	string mn = vs.readModelNumber();
	cout << "Model Number: " << mn << endl;
	// Get some orientation and IMU data.
	YawPitchRollMagneticAccelerationAndAngularRatesRegister reg;
	reg = vs.readYawPitchRollMagneticAccelerationAndAngularRates();
	cout << "Current YPR: " << reg.yawPitchRoll << endl;
	cout << "Current Magnetic: " << reg.mag << endl;
	cout << "Current Acceleration: " << reg.accel << endl;
	cout << "Current Angular Rates: " << reg.gyro << endl;

	for (int i = 0; i < 100; i++) {
		outFileStr[13] = i / 10 + '0';
		outFileStr[14] = i % 10 + '0';
		if (GetFileAttributes(outFileStr) == INVALID_FILE_ATTRIBUTES) {//File doesn't already exist
			break;
		}
	}
	
	outFile.open(outFileStr,std::ofstream::binary);

	AsciiAsync asciiAsync = (AsciiAsync) 0;
	vs.writeAsyncDataOutputType(asciiAsync); //Turns off ASCII messages
	// As an alternative to receiving notifications of new ASCII asynchronous
	// messages, the binary output configuration of the sensor is another
	// popular choice for receiving data since it is compact, fast to parse,
	// and can be output at faster rates over the same connection baudrate.
	// Here we will configure the binary output register and process packets
	// with a new callback method that can handle both ASCII and binary
	// packets.
	// First we create a structure for setting the configuration information
	// for the binary output register to send yaw, pitch, roll data out at
	//  Hz.
	//ImuRateConfigurationRegister ircr = vs.readImuRateConfiguration();
	// ircr.imuRate = 400;
	//vs.writeImuRateConfiguration(ircr);
	BinaryOutputRegister bor(
		ASYNCMODE_PORT1,
		2,
		COMMONGROUP_TIMEGPS | COMMONGROUP_YAWPITCHROLL | COMMONGROUP_ANGULARRATE | COMMONGROUP_POSITION | COMMONGROUP_VELOCITY | COMMONGROUP_INSSTATUS, // Note use of binary OR to configure flags.
		TIMEGROUP_NONE,
		IMUGROUP_TEMP | IMUGROUP_PRES,
		GPSGROUP_NONE,
		ATTITUDEGROUP_YPRU,
		INSGROUP_POSU | INSGROUP_VELU);
	vs.writeBinaryOutput1(bor);
	vs.registerAsyncPacketReceivedHandler(NULL, asciiOrBinaryAsyncMessageReceived);
	while(TRUE){
		// Wait for packets
	}
	vs.unregisterAsyncPacketReceivedHandler();
	vs.disconnect();
	return 0;
}
// This is our basic callback handler for notifications of new asynchronous
// data packets received. The userData parameter is a pointer to the data we
// supplied when we called registerAsyncPacketReceivedHandler. In this case
// we didn't need any user data so we just set this to NULL. Alternatively you
// can provide a pointer to user data which you can use in the callback method.
// One use for this is help in calling back to a member method instead of just
// a global or static method. The Packet p parameter is an encapsulation of
// the data packet. At this state, it has already been validated and identified
// as an asynchronous data message. However, some processing is required on the
// user side to make sure it is the right type of asynchronous message type so
// we can parse it correctly. The index parameter is an advanced usage item and
// can be safely ignored for now.
void asciiOrBinaryAsyncMessageReceived(void* userData, Packet& p, size_t index)
{
	if (p.type() == Packet::TYPE_ASCII && p.determineAsciiAsyncType() == VNYPR)
	{
		vec3f ypr;
		p.parseVNYPR(&ypr);
		cout << "ASCII Async YPR: " << ypr << endl;
		return;
	}
	if (p.type() == Packet::TYPE_BINARY)
	{
		// First make sure we have a binary packet type we expect since there
		// are many types of binary output types that can be configured.
		if (!p.isCompatible(
			COMMONGROUP_TIMEGPS | COMMONGROUP_YAWPITCHROLL | COMMONGROUP_ANGULARRATE | COMMONGROUP_POSITION | COMMONGROUP_VELOCITY | COMMONGROUP_INSSTATUS, // Note use of binary OR to configure flags.
			TIMEGROUP_NONE,
			IMUGROUP_TEMP | IMUGROUP_PRES,
			GPSGROUP_NONE,
			ATTITUDEGROUP_YPRU,
			INSGROUP_POSU | INSGROUP_VELU))
			// Not the type of binary packet we are expecting.
			return;
		// Ok, we have our expected binary output packet. Since there are many
		// ways to configure the binary data output, the burden is on the user
		// to correctly parse the binary packet. However, we can make use of
		// the parsing convenience methods provided by the Packet structure.
		// When using these convenience methods, you have to extract them in
		// the order they are organized in the binary packet per the User Manual.
		
		outFile.write(p.datastr().c_str(), PACKETSIZE );
	}
}
