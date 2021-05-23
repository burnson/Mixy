#!/bin/bash

#Change to the parent directory of this script.
cd `dirname $0`/..

#Ask for sudo ahead of time
echo "* This installer will use administrative privileges"
sudo echo > /dev/null

echo "* Checking for xcodebuild and /Applications/Introjucer.app"

#Make sure xcodebuild exists.
if [[ -z `which xcodebuild` ]]; then
  echo "Error: xcodebuild not found on path"
  echo
  echo "Make sure Xcode is installed"
  echo "You can get Xcode for free from the Mac App Store"
  exit
fi

#Make sure Introjucer exists.
if [ ! -f /Applications/Introjucer.app/Contents/MacOS/Introjucer ]; then
  echo "Error: Introjucer not found in Applications"
  echo
  echo "Introjucer is used to create the Xcode projects"
  echo "You can download the latest Introjucer from http://juce.com"
  exit
fi

#Trash the builds directory.
echo "* Cleaning current Builds directory"
rm -rf Builds

#Recreate the Xcode projects using the Introjucer app and build them.
echo "* Creating Mixy Xcode project with Introjucer"
/Applications/Introjucer.app/Contents/MacOS/Introjucer \
  --resave Mixy.jucer &> Installer/build.Mixy.log

echo "* Building Mixy with xcodebuild (check Installer/build.Mixy.log for details)"
xcodebuild -project Builds/MacOSX/Mixy.* \
  -target Mixy -configuration Release >> Installer/build.Mixy.log

echo "* Creating MixyServer Xcode project with Introjucer"
/Applications/Introjucer.app/Contents/MacOS/Introjucer \
  --resave MixyServer.jucer &> Installer/build.MixyServer.log

echo "* Building MixyServer with xcodebuild (check Installer/build.MixyServer.log for details)"
xcodebuild -project Builds/MacOSX/MixyServer.* \
  -target MixyServer -configuration Release >> Installer/build.MixyServer.log

#Copy the source tree to /Applications/Mixy
sudo rm -rf /Applications/Mixy
sudo mkdir /Applications/Mixy
sudo cp -rf Builds /Applications/Mixy
sudo rm -rf /Applications/Mixy/Builds/MacOSX/*.*
sudo cp -rf Installer /Applications/Mixy

#Install the application into the background
/Applications/Mixy/Installer/InstallBackground
