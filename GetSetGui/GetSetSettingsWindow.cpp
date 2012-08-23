//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by Andr� Aichert (aaichert@gmail.com)
//    
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//  
//    http://www.apache.org/licenses/LICENSE-2.0
//    
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include "GetSetSettingsWindow.h"

#include <QtGui/QTabWidget>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMenu>

// debug
#include <iostream>


void GetSetSettingsWindow::create(GetSetDictionary& dict, const std::vector<std::string>& tabs)
{
	m_tabWidget = new QTabWidget;
	for (int i=0;i<(int)tabs.size(); i++)
	{
		GetSetWidget*tab=new GetSetWidget(dict,tabs[i]);
		tab->setObjectName(tabs[i].c_str());
		m_tabWidget->addTab(tab,tabs[i].c_str());
	}
	m_tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_tabWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ctxMenu(const QPoint &)));

	m_ok=0x0;
	m_mainLayout = new QVBoxLayout;
	m_mainLayout->addWidget(m_tabWidget);

	setLayout(m_mainLayout);

}

GetSetSettingsWindow::GetSetSettingsWindow(const std::string& path, GetSetDictionary& dict ,const std::string& title, const std::string& listOfTabs, QWidget *parent)
	: QWidget(parent)
	, Access(dict)
{
	setWindowTitle(title.c_str());

	std::vector<std::string> tabs=stringToVector<std::string>(listOfTabs,';');

	if (tabs.empty())
	{
		std::vector<std::string> all;
		GetSetInternal::GetSetSection * section=dynamic_cast<GetSetInternal::GetSetSection *>(Access::getProperty(path));
		if (section)
		{
			for (GetSetInternal::GetSetSection::PropertyByName::const_iterator it=section->getSection().begin();it!=section->getSection().end();++it)
				if (dynamic_cast<GetSetInternal::GetSetSection *>(it->second))
					all.push_back(it->first);
			create(dict,all);
		}
	}
	else
		create(dict,tabs);
}

GetSetSettingsWindow::~GetSetSettingsWindow()
{
	delete m_mainLayout;
}

void GetSetSettingsWindow::ctxMenu(const QPoint &pos)
{
	QMenu menu;
	menu.addAction("Pop out");
	QAction* selectedItem = menu.exec(this->mapToGlobal(pos));
	if (selectedItem)
	{
		GetSetWidget* out=dynamic_cast<GetSetWidget*>(m_tabWidget->currentWidget());
		if (out)
		{
			std::string section=out->objectName().toAscii().data();
			GetSetWidget* w=new GetSetWidget(out->getDictionary(), section);
			w->setAttribute(Qt::WA_DeleteOnClose, true);
			w->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
			w->show();
		}
	}
}
