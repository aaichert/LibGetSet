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

#include "GetSetTabWidget.h"
#include "GetSetScriptEdit.h"
#include "../GetSet/GetSetScripting.h"

#include "../GetSet/GetSet.hxx"

#include <QTabWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

#include <QApplication>

// Minor leaks. FIXME

namespace GetSetGui
{

	void GetSetTabWidget::create(GetSetDictionary& dict, const std::string & path, const std::vector<std::string>& tabs)
	{
		if (tabs.empty())
		{
			std::vector<std::string> all;
			GetSetInternal::GetSetSection * section=dynamic_cast<GetSetInternal::GetSetSection *>(Access::getProperty(path));
			if (section)
			{
				for (GetSetInternal::GetSetSection::PropertyByName::const_iterator it=section->getSection().begin();it!=section->getSection().end();++it)
					if (dynamic_cast<GetSetInternal::GetSetSection *>(it->second))
						all.push_back(it->first);
				return create(dict,path,all);
			}
		}

		m_tabWidget = new QTabWidget;
		for (int i=0;i<(int)tabs.size(); i++)
		{
			GetSetWidget* tab=new GetSetWidget(dict,tabs[i]);
			tab->setObjectName(tabs[i].c_str());
			m_tabWidget->addTab(tab,tabs[i].c_str());
		}
		m_tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(m_tabWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ctxMenu(const QPoint &)));

		m_mainLayout = new QVBoxLayout;
		m_menuBar=new QMenuBar();
	
		m_mainLayout->setMenuBar(m_menuBar);
		m_mainLayout->addWidget(m_tabWidget);

		setLayout(m_mainLayout);

		m_script_editor=new GetSetGui::GetSetScriptEdit(this);
		std::string appname=GetSet<>("Application");
		m_script_editor->openFile((appname+".getset").c_str());


	}

	GetSetTabWidget::GetSetTabWidget(QWidget *parent)
		: QWidget(parent)
		, Access(GetSetDictionary::global())
		, m_menuBar(0x0)
	{
		setWindowTitle("Settings");
		create(GetSetDictionary::global(),"",std::vector<std::string>());
	}

	GetSetTabWidget::GetSetTabWidget(const std::string& path, GetSetDictionary& dict ,const std::string& title, const std::string& listOfTabs, QWidget *parent)
		: QWidget(parent)
		, Access(dict)
		, m_menuBar(0x0)
	{
		setWindowTitle(title.c_str());
		std::vector<std::string> tabs=stringToVector<std::string>(listOfTabs,';');

		create(dict,path,tabs);
	}

	void GetSetTabWidget::setCallBack(void (*gui)(const std::string& sender, const std::string& action))
	{
		callback=gui;
	}

	QAction* GetSetTabWidget::addMenuItem(const std::string& menu, const std::string& action, const std::string& shortcut)
	{
		// First create menu structure
		if (m_menus.find(menu)==m_menus.end())
		{
			std::vector<std::string> submenus=stringToVector<std::string>(menu,'/');
			std::string this_menu=submenus.back();
			submenus.pop_back();
			m_menus[menu]=new QMenu(this_menu.c_str(),this);
			if (submenus.empty())
			{
				m_menuBar->addMenu(m_menus[menu]);
			}
			else
			{
				std::string super_menu=vectorToString(submenus,"/");
				if (m_menus.find(super_menu)==m_menus.end())
					addMenuItem(super_menu,"");
				m_menus[super_menu]->addMenu(m_menus[menu]);
			}

		}
		// Then create menu item
		if (!action.empty())
		{
			QAction* item=m_menus[menu]->addAction(action.c_str(), this, SLOT(handle_action()), QKeySequence(shortcut.c_str()));
			item->setObjectName(action.c_str());
			return item;
		}
		else return 0x0;
	}

	QPushButton* GetSetTabWidget::addButton(const std::string& action)
	{
		if (m_push_buttons.find(action)==m_push_buttons.end())
		{
			QPushButton *pb=new QPushButton(action.c_str(),this);
			pb->setObjectName(action.c_str());
			connect(pb, SIGNAL(clicked()), this, SLOT(handle_action()) );
			m_mainLayout->addWidget(pb);
		}
		return m_push_buttons[action];
	}

	void GetSetTabWidget::addDefaultFileMenu()
	{
		addMenuItem("File","");
		m_menus["File"]->addAction(tr("&About"), this, SLOT(about()));
		auto *scripting_menu=m_menus["File"]->addMenu("Scripting");
		scripting_menu->addAction(tr("(Re-)Start Recording"), this, SLOT(rec_start()) );
		scripting_menu->addAction(tr("Stop Recording"), this, SLOT(rec_stop()) );
		scripting_menu->addSeparator();
		scripting_menu->addAction(tr("Run default script."), this, SLOT(script_run_default()), QKeySequence(Qt::CTRL + Qt::SHIFT+ Qt::Key_D));
		scripting_menu->addAction(tr("Show Script &Editor..."), this, SLOT(script_editor()), QKeySequence(Qt::CTRL + Qt::Key_E));
		m_menus["File"]->addSeparator();
		m_menus["File"]->addAction(tr("&Quit"), QApplication::instance(), SLOT(quit()), QKeySequence::Quit);
	}


	void GetSetTabWidget::handle_action()
	{
		std::string who=windowTitle().toStdString();
		std::string what=sender()->objectName().toStdString();
		if (callback) callback(who,what);

	}

	void GetSetTabWidget::about()
	{
		QMessageBox::about( this, "GetSet c++ Library",
			"<h4>Load/saving *typed* and *named* properties and automatic GUI</h4>\n\n"
			"Copyright 2011-2016 by <a href=\"mailto:aaichert@gmail.com?Subject=GetSet\">Andre Aichert</a> <br><br> See slso: "
			"<a href=\"https://sourceforge.net/projects/getset/\">SourceForge Project Page</a> <br><br>"
			"<h4>Licensed under the Apache License, Version 2.0 (the \"License\")</h4>\n\n"
			"You may not use this file except in compliance with the License. You may obtain a copy of the License at "
			"<a href=\"http://www.apache.org/licenses/LICENSE-2.0\">http://www.apache.org/licenses/LICENSE-2.0</a><br>"
			);
	}

	void GetSetTabWidget::script_editor()
	{
		m_script_editor->show();
	}

	void GetSetTabWidget::rec_start()
	{
	
	}
	
	void GetSetTabWidget::rec_stop()
	{
	
	}
	
	void GetSetTabWidget::script_run_default()
	{
		std::string appname=GetSet<>("Application");
		std::cout << "Running script " << appname << ".getset" << std::endl;
		GetSetScriptParser::global().parse(std::string("file run ")+appname+".getset");
	}

	GetSetTabWidget::~GetSetTabWidget()
	{
		delete m_mainLayout;
	}

	void GetSetTabWidget::ctxMenu(const QPoint &pos)
	{
		QMenu menu;
		menu.addAction("Pop out");
		QAction* selectedItem = menu.exec(this->mapToGlobal(pos));
		if (selectedItem)
		{
			GetSetWidget* out=dynamic_cast<GetSetWidget*>(m_tabWidget->currentWidget());
			if (out)
			{
				std::string section=out->objectName().toLatin1().data();
				GetSetWidget* w=new GetSetWidget(out->getDictionary(), section, this);
				w->setAttribute(Qt::WA_DeleteOnClose, true);
				w->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
				w->show();
			}
		}
	}

} // namespace GetSetGui
