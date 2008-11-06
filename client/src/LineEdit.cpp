/******************************************************************************\
*  libkawaii - A Japanese language support library for Qt4                     *
*  Copyright (C) 2007 John Eric Martin <john.eric.martin@gmail.com>            *
*                                                                              *
*  This program is free software; you can redistribute it and/or modify        *
*  it under the terms of the GNU General Public License version 2 as           *
*  published by the Free Software Foundation.                                  *
*                                                                              *
*  This program is distributed in the hope that it will be useful,             *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
*  GNU General Public License for more details.                                *
*                                                                              *
*  You should have received a copy of the GNU General Public License           *
*  along with this program; if not, write to the                               *
*  Free Software Foundation, Inc.,                                             *
*  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                   *
\******************************************************************************/

#include "LineEdit.h"
#include "KanaConversion.h"

#include <QtGui/QKeyEvent>

/// @TODO Fix it from changing selection when the input box is clicked
KawaiiLineEdit::KawaiiLineEdit(QWidget *parent) : SearchEdit(parent), mRomajiMode(false), mInsertPosition(0), mDisplayMode(Hiragana)
{
	connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(updateText()));
};

KawaiiLineEdit::KawaiiLineEdit(const QString& contents, QWidget *parent) : SearchEdit(parent), mRomajiMode(false), mInsertPosition(0), mDisplayMode(Hiragana)
{
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateText()));
};

bool KawaiiLineEdit::romajiMode()
{
	return mRomajiMode;
};

void KawaiiLineEdit::setRomajiMode(bool enabled)
{
	bool block = blockSignals(true);

	if(enabled)
	{
		// If we are switching, process the text already in the box
		if(!mRomajiMode)
		{
			mRealText = text();
		}
	}
	else
	{
		// If we are switching from romaji mode, move the real text into the box
		if(mRomajiMode)
		{
			setText(mRealText);
			mRealText.clear();
		}
	}

	mActiveText.clear();
	mDisplayText.clear();

	mInsertPosition = 0;
	mDisplayMode = Hiragana;

	deselect();
	setCursorPosition( text().length() );

	blockSignals(block);

	mRomajiMode = enabled;
	updateText();
};

void KawaiiLineEdit::updateText()
{
	if(!mRomajiMode)
		return;

	bool block = blockSignals(true);

	// mDisplayText = romajiToKana(mActiveText);
	if(mDisplayMode == Hiragana)
		mDisplayText = katakanaToHiragana( romajiToKana(mActiveText) );
	else
		mDisplayText = hiraganaToKatakana( romajiToKana(mActiveText) );

	QString temp = mRealText;
	temp.insert(mInsertPosition, mDisplayText);
	setText( temp );
	setSelection( mInsertPosition, mDisplayText.length() );
	// setText( mRealText + mDisplayText );
	// setSelection( mRealText.length(), mDisplayText.length() );

	blockSignals(block);

	if( mActiveText.isEmpty() )
		emit textChanged(mRealText);
};

void KawaiiLineEdit::keyPressEvent(QKeyEvent *event)
{
	if(!mRomajiMode)
		return QLineEdit::keyPressEvent(event);

	if(event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
	{
		if( mActiveText.isEmpty() )
		{
			if( !selectedText().isEmpty() )
			{
				mRealText.remove(selectionStart(), selectedText().length());
				mInsertPosition = selectionStart();
			}
			else if(event->key() == Qt::Key_Delete) // Delete after the cursor
			{
				mRealText.remove(cursorPosition(), 1);
				mInsertPosition = cursorPosition();
			}
			else if(cursorPosition() > 0) // Delete the character before the cursor
			{
				mRealText.remove(cursorPosition() - 1, 1);
				mInsertPosition = cursorPosition() - 1;
			}

			event->accept();
			updateText();
			return;
		}

		// Remove the last character in the string (if there is one)
		mActiveText.chop(1);

		event->accept();
		updateText();

		return;
	}
	else if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
	{
		if( mActiveText.isEmpty() )
			return QLineEdit::keyPressEvent(event);

		mRealText.insert(mInsertPosition, mDisplayText);
		mInsertPosition += mDisplayText.length();
		mDisplayMode = Hiragana;

		mDisplayText.clear();
		mActiveText.clear();

		event->accept();
		updateText();

		setCursorPosition(mInsertPosition);

		return;
	}
	else if( !mActiveText.isEmpty() && (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right ||
		event->key() == Qt::Key_Home || event->key() == Qt::Key_End) )
	{
		event->accept();
		return;
	}
	else if( !mActiveText.isEmpty() && event->key() == Qt::Key_F7)
	{
		mDisplayMode = (mDisplayMode == Hiragana) ? Katakana : Hiragana;
		event->accept();
		updateText();
		return;
	}

	// Normal key, clear the text first
	if( !event->text().isEmpty() )
	{
		if( mActiveText.isEmpty() )
			mInsertPosition = cursorPosition();

		// If there is a selection, delete it
		if( !selectedText().isEmpty() && mActiveText.isEmpty() )
		{
			mRealText.remove(selectionStart(), selectedText().length());
			mInsertPosition = selectionStart();
		}

		mActiveText += event->text();
		event->accept();
		updateText();
		return;
	}

	QLineEdit::keyPressEvent(event);
};

void KawaiiLineEdit::paintEvent(QPaintEvent *event)
{
	QLineEdit::paintEvent(event);
};
