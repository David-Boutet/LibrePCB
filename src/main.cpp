/*
 * EDA4U - Professional EDA for everyone!
 * Copyright (C) 2013 Urban Bruhin
 * http://eda4u.ubruhin.ch/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/

#include <QtCore>
#include <QApplication>
#include <QTranslator>
#include "common/exceptions.h"
#include "workspace/workspace.h"
#include "workspace/workspacechooserdialog.h"

/*****************************************************************************************
 *  main()
 ****************************************************************************************/

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // this is to remove the ugly frames around widgets in all status bars...
    // (from http://www.qtcentre.org/threads/1904)
    app.setStyleSheet("QStatusBar::item { border: 0px solid black; }");

    QCoreApplication::setOrganizationName("EDA4U");
    //QCoreApplication::setOrganizationDomain(""); ///< @todo
#ifdef GIT_BRANCH
    QCoreApplication::setApplicationName(QString("EDA4U_git-%1").arg(GIT_BRANCH));
#else
    QCoreApplication::setApplicationName("EDA4U");
#endif
    QCoreApplication::setApplicationVersion("0.0.1");

    // Install Qt translations
    QTranslator qtTranslator;
    qtTranslator.load("qt_" % QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // Install system language translations (all system languages defined in the system settings, in the defined order)
    QTranslator systemTranslator;
    systemTranslator.load(QLocale::system(), "eda4u", "_", ":/i18n/");
    app.installTranslator(&systemTranslator);

    // Install language translations (like "de" for German)
    QTranslator appTranslator1;
    appTranslator1.load("eda4u_" % QLocale::system().name().split("_").at(0), ":/i18n");
    app.installTranslator(&appTranslator1);

    // Install language/country translations (like "de_ch" for German/Switzerland)
    QTranslator appTranslator2;
    appTranslator2.load("eda4u_" % QLocale::system().name(), ":/i18n");
    app.installTranslator(&appTranslator2);

    QGuiApplication::setQuitOnLastWindowClosed(false);

    // Initialization finished, open the workspace...

    QDir workspaceDir;
    workspaceDir.setPath(Workspace::getMostRecentlyUsedWorkspacePath());

    if ((!workspaceDir.exists()) || (!Workspace::isValidWorkspaceDir(workspaceDir)))
    {
        WorkspaceChooserDialog dialog;

        if (!dialog.exec())
            return 0; // no workspace was choosed

        workspaceDir = dialog.getChoosedWorkspaceDir();
    }

    Workspace ws(workspaceDir);
    ws.showControlPanel();

    // please note that we shouldn't show a dialog or message box in the catch() blocks!
    // from http://qt-project.org/doc/qt-5/exceptionsafety.html:
    //      "After an exception is thrown, the connection to the windowing server might
    //      already be closed. It is not safe to call a GUI related function after
    //      catching an exception."
    try
    {
        return app.exec();
    }
    catch (Exception& e)
    {
        qFatal("UNCAUGHT EXCEPTION: %s --- PROGRAM EXITED", e.getDebugString().toUtf8().constData());
    }
    catch (std::exception& e)
    {
        qFatal("UNCAUGHT EXCEPTION: %s --- PROGRAM EXITED", e.what());
    }
    catch (...)
    {
        qFatal("UNCAUGHT EXCEPTION --- PROGRAM EXITED");
    }

    return -1;
}