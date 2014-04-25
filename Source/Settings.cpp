#include "Settings.h"

const String addressKey ("Address");
const String portKey ("Port");

Settings::Settings()
{
	PropertiesFile::Options options;
	options.applicationName     = "WorkbenchRemote";
	options.folderName          = "EchoAVB";
	options.filenameSuffix      = "xml";
	options.osxLibrarySubFolder = "Application Support";
	propfile = new PropertiesFile (options);
}

Settings::~Settings()
{

}

void Settings::storeAddress( IPAddress address )
{
	propfile->setValue(addressKey, address.toString());

}
void Settings::storePort( int port )
{
	propfile->setValue(portKey, port);

}

int Settings::getPort()
{
	return propfile->getIntValue(portKey, 0);
}

IPAddress Settings::getAddress()
{
	return IPAddress(propfile->getValue(addressKey, "127.0.0.1"));
}
