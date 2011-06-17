//QDialog for Saving Cell Export files (SWC, JPG)
#ifndef CELLEXPORT_H
#define CELLEXPORT_H

#include <iostream>
#include <QtGui>
#include <QApplication>

class SaveCellExportDialog : public QDialog
{
	Q_OBJECT
	
public:
	SaveCellExportDialog(QWidget* parent, QString curdirectoryswc, QString curdirectoryjpg, QString swcfileName, QString jpgfileName, bool changeswcfileName, bool changejpgfileName);

	QString getSWCDir();
	QString getJPGDir();
	QString getSWCfileName();
	QString getJPGfileName();
	bool keeporiginalSWCfileName();
	bool keeporiginalJPGfileName();
	
private slots:	
	void swcBrowse();
	void jpgBrowse();
	void swcfilenaming();
	void jpgfilenaming();
	void save();

private:
	QPushButton *createButton(const QString &text, const char *member);
	QComboBox *createComboBox(const QString &text = QString());

	QGroupBox *saveSWCGroupBox;
	QGroupBox *saveJPGGroupBox;
//set up for swc files
	QComboBox *swcdirectoryComboBox;
	QPushButton *swcbrowseButton;
	QPushButton *swcmoreButton;
	QWidget *swcextension;
	QRadioButton *originalswcfileNameButton, *renumberswcfileNameButton, *renameswcfileNameButton;
	QLineEdit *nameswcfileNameLine;
//set up for jpg files
	QComboBox *jpgdirectoryComboBox;
	QPushButton *jpgbrowseButton;
	QPushButton *jpgmoreButton;
	QWidget* jpgextension;
	QRadioButton *originaljpgfileNameButton, *renumberjpgfileNameButton, *renamejpgfileNameButton;
	QLineEdit *namejpgfileNameLine;\

	QPushButton *OkButton;
	QPushButton *CancelButton;

	//variables to return to the caller
	QString curdirectoryswc;
	QString curdirectoryjpg;
	QString swcfileName;
	QString jpgfileName;
	bool changeswcfileName;
	bool changejpgfileName;
	
};

#endif