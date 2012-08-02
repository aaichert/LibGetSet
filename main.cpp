#include <QtGui/qapplication>

#include "GetSet/GetSet.hxx"
#include "GetSetGui/GetSetSettingsWindow.h"

#include <iostream>

void gui(const std::string& section, const std::string& key)
{
	if (key=="XML")
		std::cout << GetSetDictionary::globalDictionary().getXML();

	std::cout << section << " : " << key << "=" << GetSet<std::string>(section,key).getString() << std::endl;
}

int main(int argc, char** argv)
{
	// Basic GetSet usage:
	GetSet<int>("Test 1", "Number of Iterations")=13;
	int nIterations=GetSet<int>("Test 1", "Number of Iterations"); // == 13
	
	//
	// Special types, such as Directory, File, Slider, Enum, Trigger etc...
	//
	
	// Creating a Slider:
	GetSetGui::Slider("Test 1", "Some Double Value").setMin(-5);
	GetSetGui::Slider("Test 1", "Some Double Value").setMax(2.1);

	// Creating a button
	GetSetGui::Trigger("Test 1", "XML")="Print to cout";

	// Files and directories
	GetSetGui::Directory("Test 2", "Some Directory");	
	GetSetGui::File out("Test 2", "Output File");
	out.setExtensions("Images (*.jpg *.png)");
	out.setCreateNew(true);

	// Setting Values via GetSet<...>, also for special types
	GetSet<std::string>("Test 2", "Some Directory")="./out";
	GetSet<double>("Test 1", "Some Double Value")=0.5;
	
	//GetSetCmdLineParser cmd/*(dictionary,autoAll)*/;
	//cmd.flagShort("o","Test 2","Some Directory");
	//cmd.flagLong("output-dir","Test 2","Some Directory");
	//cmd.flagAuto("Test 1","Some Directory", addSectionName);
	//cmd.flagAutoSection("Test 1", addSectionName);
	//cmd.flagUnnamed(0,"Test 1","Some Directory");
	//cmd.require<std::string>("Test 1","Some Directory");
	//bool okay=cmd.parse(argc,argv);
	//cmd.getUnnamedArgs()[2];
	//cmd.getUnhandledArgs()[0];
	//cmd.parse();
	//cmd.getXML();


	//
	// Listing to Events
	//
	GetSetHandler callback(gui);

	//
	// Automatically generate a GUI (in this case using Qt)
	//
	
	QApplication app(argc,argv);

	GetSetSettingsWindow setup;
	setup.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	setup.show();

	return app.exec();
}
