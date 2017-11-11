# configurations
if(Debug)
	include_directories(
	)

	add_definitions(
	)

	add_executable(wxgui
		../dll/zenwinx/guicon.cpp
		about.cpp
		btd.cpp
		config.cpp
		crash.cpp
		fileslist.cpp
		i18n.cpp
		job.cpp
		LCN.cpp
		log.cpp
		main.cpp
		map.cpp
		menu.cpp
		prec.cpp
		query.cpp
		report.cpp
		shutdown.cpp
		SimpleTransientPopup.cpp
		statbar.cpp
		taskbar.cpp
		toolbar.cpp
		tray.cpp
		upgrade.cpp
		utils.cpp
		vollist.cpp
	)

	link_directories(
		
	)

endif()

if(Debug)
	include_directories(
	)

	add_definitions(
	)

	add_executable(wxgui
		../dll/zenwinx/guicon.cpp
		about.cpp
		btd.cpp
		config.cpp
		crash.cpp
		fileslist.cpp
		i18n.cpp
		job.cpp
		LCN.cpp
		log.cpp
		main.cpp
		map.cpp
		menu.cpp
		prec.cpp
		query.cpp
		report.cpp
		shutdown.cpp
		SimpleTransientPopup.cpp
		statbar.cpp
		taskbar.cpp
		toolbar.cpp
		tray.cpp
		upgrade.cpp
		utils.cpp
		vollist.cpp
	)

	link_directories(
		
	)

endif()

if(Release)
	include_directories(
	)

	add_definitions(
		-DOFFICIAL_RELEASE
		-D\"$(INHERIT)\"
	)

	add_executable(wxgui
		../dll/zenwinx/guicon.cpp
		about.cpp
		btd.cpp
		config.cpp
		crash.cpp
		fileslist.cpp
		i18n.cpp
		job.cpp
		LCN.cpp
		log.cpp
		main.cpp
		map.cpp
		menu.cpp
		prec.cpp
		query.cpp
		report.cpp
		shutdown.cpp
		SimpleTransientPopup.cpp
		statbar.cpp
		taskbar.cpp
		toolbar.cpp
		tray.cpp
		upgrade.cpp
		utils.cpp
		vollist.cpp
	)

	link_directories(
		
	)

endif()

if(Release)
	include_directories(
	)

	add_definitions(
		-DOFFICIAL_RELEASE
		-D\"$(INHERIT)\"
	)

	add_executable(wxgui
		../dll/zenwinx/guicon.cpp
		about.cpp
		btd.cpp
		config.cpp
		crash.cpp
		fileslist.cpp
		i18n.cpp
		job.cpp
		LCN.cpp
		log.cpp
		main.cpp
		map.cpp
		menu.cpp
		prec.cpp
		query.cpp
		report.cpp
		shutdown.cpp
		SimpleTransientPopup.cpp
		statbar.cpp
		taskbar.cpp
		toolbar.cpp
		tray.cpp
		upgrade.cpp
		utils.cpp
		vollist.cpp
	)

	link_directories(
		
	)

endif()

