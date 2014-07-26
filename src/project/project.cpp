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
#include "project.h"
#include "../common/exceptions.h"
#include "../workspace/workspace.h"
#include "library/projectlibrary.h"
#include "circuit/circuit.h"
#include "schematics/schematiceditor.h"

namespace project {

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

Project::Project(Workspace* workspace, const QString& filename) :
    QObject(0), mWorkspace(workspace), mFileInfo(filename), mUndoStack(0)
{
    if ((!mFileInfo.exists()) || (!mFileInfo.isFile()) || (mFileInfo.suffix() != "e4u"))
        throw RuntimeError(QString("Invalid project file: \"%1\"").arg(filename), __FILE__, __LINE__);

    mUndoStack = new QUndoStack(0);
    mProjectLibrary = new ProjectLibrary(mWorkspace, this);
    mCircuit = new Circuit(mWorkspace, this);
    mSchematicEditor = new SchematicEditor(mWorkspace, this);
}

Project::~Project()
{
    mWorkspace->unregisterOpenProject(this); // inform the workspace that this project will get destroyed

    delete mSchematicEditor;        mSchematicEditor = 0;
    delete mCircuit;                mCircuit = 0;
    delete mProjectLibrary;         mProjectLibrary = 0;
    delete mUndoStack;              mUndoStack = 0;
}

/*****************************************************************************************
 *  General Methods
 ****************************************************************************************/

bool Project::windowIsAboutToClose(QMainWindow* window)
{
    int countOfOpenWindows = mSchematicEditor->isVisible();

    if (countOfOpenWindows <= 1)
    {
        // the last open window (schematic editor, board editor, ...) is about to close.
        // --> close the whole project
        return close(window);
    }

    return true; // this is not the last open window, so no problem to close it...
}

/*****************************************************************************************
 *  Public Slots
 ****************************************************************************************/

void Project::showSchematicEditor()
{
    mSchematicEditor->show();
    mSchematicEditor->raise();
}

void Project::save()
{
    if (mUndoStack->isClean())
        return;

    // todo: save project
}

bool Project::close(QWidget* messageBoxParent)
{
    if (mUndoStack->isClean())
    {
        // no unsaved changes --> the project can be closed
        deleteLater();  // this project object will be deleted later in the event loop
        return true;
    }

    if (!messageBoxParent)
    {
        if (mSchematicEditor->isVisible())
            messageBoxParent = mSchematicEditor;
    }

    QMessageBox::StandardButton choice = QMessageBox::question(messageBoxParent,
         tr("Save Project?"), tr("You have unsaved changes in the project.\n"
         "Do you want to save them bevore closing the project?"),
         QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

    switch (choice)
    {
        case QMessageBox::Yes: // save and close project
            save();
            deleteLater();  // this project object will be deleted later in the event loop
            return true;

        case QMessageBox::No: // close project without saving
            deleteLater();  // this project object will be deleted later in the event loop
            return true;

        default: // cancel, don't close the project
            return false;
    }
}

/*****************************************************************************************
 *  Static Methods
 ****************************************************************************************/

QString Project::uniqueProjectFilename(const QString& filename)
{
    QFileInfo fileInfo(filename);
    QString uniqueFilename = QDir::toNativeSeparators(fileInfo.canonicalFilePath());
    if (uniqueFilename.isEmpty())
        throw RuntimeError(QString("Invalid filename: \"%1\"").arg(filename), __FILE__, __LINE__);
    return uniqueFilename;
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project




