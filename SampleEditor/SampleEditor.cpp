/*=========================================================================
Copyright 2009 Rensselaer Polytechnic Institute
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. 
=========================================================================*/

#include "SampleEditor.h"
//#include "Dendrogram.h"
#include "../SPD/SPDAnalysisModel.h"


//*******************************************************************************
// SampleEditor
//********************************************************************************

SampleEditor::SampleEditor(QWidget * parent, Qt::WindowFlags flags)
: QMainWindow(parent,flags)
{
	table = new TableWindow();
	plot = new PlotWindow(this);
	histo = new HistoWindow(this);
	graph =  new GraphWindow(this);
	dendro1 = new Dendrogram(this);
	dendro2 = new Dendrogram(this);////////////////////////////////////////////////////////////////
	heatmap = new Heatmap(this);

	data = NULL;
	data = vtkSmartPointer<vtkTable>::New();		//Start with a new table

	selection = new ObjectSelection();
	selection2 = new ObjectSelection();/////////////////////////////////////

	lastPath = ".";
	

	createMenus();
	createStatusBar();
	this->flag = 0;

	setCentralWidget(table);
	setWindowTitle(tr("Sample Editor"));
	connect(selection, SIGNAL(changed()), this, SLOT(updateStatistics()));
    
	this->resize(500,500);
}

//******************************************************************************
//Reimplement closeEvent to also close all other windows in the application
//******************************************************************************
void SampleEditor::closeEvent(QCloseEvent *event)
{
	//Then Close all other windows
	foreach (QWidget *widget, qApp->topLevelWidgets()) 
	{
		if (this != widget)
		{
			if(widget->isVisible())
				widget->close();
		}
    }
	//Then close myself
	event->accept();
}

//******************************************************************************
// Here we just show a message in the status bar when loading
//******************************************************************************
void SampleEditor::createStatusBar()
{
    QLabel *statusLabel = new QLabel(tr(" Ready"));
    statusBar()->addWidget(statusLabel, 1);
}

/*----------------------------------------------------------------------------*/
/* function: createMenus()                                                    */
/*                                                                            */
/* This function is used to create Menus that are associated with various     */
/* functions in the application. In order to add a menu, we need to do the    */
/* following:                                                                 */
/* 1.) Define a QMenu type (e.g., QMenu *fileMenu) and add it to menuBar()    */
/* 2.) Define QAction elements (e.g., QAction *openAction) associated with    */
/*     each QMenu                                                             */
/* 3.) Add a separator (menuBar()->addSeparator() after each menu group       */
/*																			  */
/*In order to create an Action, we need to do the							  */
/* following:                                                                 */
/* 1.) Define a QAction (e.g., QAction *openAction)                           */
/* 2.) Label the QAction element (e.g., openAction = new QAction(QIcon(":src/ */
/*     images/open.png"), tr("&Open..."), this). The QIcon argumenet is       */
/*     optional.                                                              */
/* 3.) Add optional "setShortcut" and "setStatusTip".                         */
/* 4.) Finally, bind this item with a "connect" that essentially calls the    */
/*     module to implement the operation (e.g.,                               */
/*     connect(openAction, SIGNAL(triggered()), this, SLOT(loadImage())). In  */
/*     this example, "loadImage()" is the module that is being called. These  */
/*     modules should be defined as "private" operators in the main class.    */
/*     The actual routines performing the operations (e.g., an image          */
/*     thresholding operation) must be accessed from within the called module.*/
/*	   Finally, after all these action's, we bind them to a "QActionGroup".       */ 
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void SampleEditor::createMenus()
{
	//FIRST HANDLE FILE MENU
	fileMenu = menuBar()->addMenu(tr("&File"));
	loadAction = new QAction(tr("Load File..."), this);
	loadAction->setStatusTip(tr("Load to table from text file"));
	connect(loadAction, SIGNAL(triggered()), this, SLOT(loadFile()));
	fileMenu->addAction(loadAction);

	editMenu = menuBar()->addMenu(tr("&Edit"));
	removeRowsAction = new QAction(tr("Remove Selected Rows"),this);
	removeRowsAction->setStatusTip(tr("Remove selected rows from the table"));
	connect(removeRowsAction, SIGNAL(triggered()), this, SLOT(removeRows()));
	editMenu->addAction(removeRowsAction);

	showStatisticsAction = new QAction(tr("Show Statistics Toolbar"), this);
	connect(showStatisticsAction,SIGNAL(triggered()), this, SLOT(showStatistics()));
	editMenu->addAction(showStatisticsAction);

	updateStatisticsAction = new QAction(tr("Update Statistics"), this);
	connect(updateStatisticsAction, SIGNAL(triggered()), this, SLOT(updateStatistics()));
	//connect((this->selection), SIGNAL(selectionChanged()), this, SLOT(updateStatistics()));
	editMenu->addAction(updateStatisticsAction);

	addBlankRowAction = new QAction(tr("Add Blank Row"), this);
	addBlankRowAction->setStatusTip(tr("Add blank row to bottom of table"));
	connect(addBlankRowAction, SIGNAL(triggered()), this, SLOT(addBlankRow()));
	editMenu->addAction(addBlankRowAction);

	changeRowDataAction = new QAction(tr("Change Row Data..."), this);
	changeRowDataAction->setStatusTip(tr("Change data for selected row"));
	connect(changeRowDataAction, SIGNAL(triggered()), this, SLOT(changeRowData()));
	editMenu->addAction(changeRowDataAction);

	SPDAction = new QAction(tr("SPD"), this);
	SPDAction->setStatusTip(tr("SPD Analysis for the data"));
	connect(SPDAction, SIGNAL(triggered()), this, SLOT(SPDAnalysis()));
	editMenu->addAction(SPDAction);

	//////////////////////////////////////////////////////////////////////////////
	ClusClusMenu = editMenu->addMenu(tr("&ClusClus"));
	sampleDendroAction = new QAction(tr("SampleDendrogram"), this);
	sampleDendroAction->setStatusTip(tr("SampleDandrogram"));
	connect(sampleDendroAction, SIGNAL(triggered()), this, SLOT(sampledendrogram()));
	ClusClusMenu->addAction(sampleDendroAction);

	featureDendroAction = new QAction(tr("FeatureDendrogram"), this);
	featureDendroAction->setStatusTip(tr("FeatureDandrogram"));
	connect(featureDendroAction, SIGNAL(triggered()), this, SLOT(featuredendrogram()));
	ClusClusMenu->addAction(featureDendroAction);

	heatmapAction = new QAction(tr("Heatmap"), this);
	heatmapAction->setStatusTip(tr("Heatmap"));
	connect(heatmapAction, SIGNAL(triggered()), this, SLOT(showheatmap()));
	ClusClusMenu->addAction(heatmapAction);
}

//********************************************************************************
// LoadFile()
//
// Ask for the header file and the data file that define a table and load the data
// into vtkTable, then show results.
//********************************************************************************
void SampleEditor::loadFile()
{
	QString headername = QFileDialog::getOpenFileName(this,"Choose a Header File", lastPath, tr("TXT Files (*.txt)") );
	if(headername == "") return;
	lastPath = QFileInfo(headername).absolutePath();

	/*QString dataname = QFileDialog::getOpenFileName(this,"Choose a Data File", lastPath, tr("TXT Files (*.txt)") );
	if(dataname == "") return;
	lastPath = QFileInfo(dataname).absolutePath();
	
	selection->clear();

	ReadFiles(headername.toStdString(), dataname.toStdString());*/
	this->data = ftk::LoadTable(headername.toStdString());
	table->setModels(data,selection,selection2);//////////////////////////////////////////////////////////////////////////
	table->show();

	plot->setModels(data,selection);
	plot->show();
	this->histo->setModels(data, selection);
	this->histo->show();
	std::cout << "I reached here inside the sample editor"<<std::endl;
	this->graph->setModels(data, selection);
	this->dendro1->setModels(data,selection);
	this->dendro2->setModels(data,selection2);//////////////////////////////////////////////
	//this->heatmap->setModels(data,selection,selection2);

}


void SampleEditor::ReadFiles(std::string hname, std::string dname)
{

	const int MAXLINESIZE = 102400;	//Numbers could be in scientific notation in this file
	char line[MAXLINESIZE];

	//data = vtkSmartPointer<vtkTable>::New();		//Start with a new table
	data->Initialize();
	std::cout << "The value of hname is "<<hname<<std::endl;
	std::cout << "The value of dname is "<<dname <<std::endl;

	//LOAD THE HEADER INFO:
	ifstream headerFile; 
	headerFile.open( hname.c_str() );
	if ( !headerFile.is_open() )
		return ;

	vtkSmartPointer<vtkDoubleArray> column = vtkSmartPointer<vtkDoubleArray>::New();
	headerFile.getline(line, MAXLINESIZE);
	/*std::cout << line << std::endl;*/
	while ( !headerFile.eof() ) //Get all values
	{
		std::string h;
		char * pch = strtok (line," \t");
		while (pch != NULL)
		{
			h = pch;
			pch = strtok (NULL, " \t");
		}
		column = vtkSmartPointer<vtkDoubleArray>::New();
		column->SetName( h.c_str() );
		data->AddColumn(column);
		/*std::cout<<"The headers is:"<<std::endl;*/
		/*data->Dump(3);*/
		headerFile.getline(line, MAXLINESIZE);
	}
	headerFile.close();
	/*std::cout << "Finished loading headers" << std::endl;*/

	//LOAD ALL OF THE FEATURES INFO:
	ifstream featureFile; 
	featureFile.open( dname.c_str() );
	if ( !featureFile.is_open() )
		return;

	featureFile.getline(line, MAXLINESIZE);
	
	std::cout << line<<std::endl;
	while ( !featureFile.eof() ) //Get all values
	{
		vtkSmartPointer<vtkVariantArray> row = vtkSmartPointer<vtkVariantArray>::New();
		char * pch = strtok (line," \t");
		while (pch != NULL)
		{
			row->InsertNextValue( vtkVariant( atof(pch) ) );
			pch = strtok (NULL, " \t");
		}
		data->InsertNextRow(row);
		/*std::cout <<"The data is:"<<std::endl;*/
		/*data->Dump(3);*/

		featureFile.getline(line, MAXLINESIZE);
	}
	featureFile.close();
	/*std::cout << "Finished loading data" << std::endl;*/
	
}

//***********************************************************************************
// This functions demonstrates how to remove rows from the table and how 
// to make sure that the views are updated accordingly.
//
// It is important to realize that row numbers change after removing a row.
// This means you cannot iterate through the table to remove rows, but 
// must look for each row independently.
//
// Also note that it is important to remove the ids of removed rows from selections.
//***********************************************************************************
void SampleEditor::removeRows(void)
{
	if( this->data->GetNumberOfRows() <= 0)
	{
		return;
	}

	std::set<long int> sels = selection->getSelections();
	std::set<long int>::iterator it;
	for(it=sels.begin(); it!=sels.end(); ++it)
	{
		for(int i=0; i<data->GetNumberOfRows(); ++i)
		{
			if( data->GetValue(i,0).ToLong() == (*it) )
			{
				data->RemoveRow(i);
				break;
			}
		}
	}
	selection->clear();
	table->update();
	plot->update();
}

void SampleEditor::addBlankRow(void)
{
	if( this->data->GetNumberOfRows() <= 0)
	{
		return;
	}
	//Get the maximum ID:
	long max = 0;
	for(int i=0; i<data->GetNumberOfRows(); ++i)
	{
		long val = data->GetValue(i,0).ToLong();
		if( val > max )
			max = val;
	}

	//Insert blank row:
	data->InsertNextBlankRow();

	long newID = max+1;

	//Set the ID of the new ROW:
	data->SetValue( data->GetNumberOfRows() - 1, 0, vtkVariant(newID));

	//Select the new row only:
	selection->select(newID);
	
	//update views:
	table->update();
	plot->update();
}

void SampleEditor::changeRowData(void)
{
	if( this->data->GetNumberOfRows() <= 0)
	{
		return;
	}
}

void SampleEditor::showStatistics(void)
{
	if( this->data->GetNumberOfRows() <= 0)
	{
		return;
	}

	this->statisticsDockWidget = new QDockWidget();
	this->statisticsToolbar = new StatisticsToolbar(statisticsDockWidget);
	
	statisticsDockWidget->setWidget(statisticsToolbar->statisticsDockWidget);
	statisticsDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
	addDockWidget(Qt::BottomDockWidgetArea, statisticsToolbar->statisticsDockWidget);

	SampleEditor::statisticsToolbar->setTable(data, selection);
	this->flag = 1;	
}

void SampleEditor::updateStatistics(void)
{
	if( this->data->GetNumberOfRows() <= 0)
	{
		return;
	}

	if (this->flag == 1)
	{
		std::cout<< "updattteeeee" << std::endl;
		statisticsToolbar->statisticsDockWidget->close();
		showStatistics();
	}
		
}

void SampleEditor::SPDAnalysis()
{
	if( this->data->GetNumberOfRows() <= 0)
	{
		return;
	}

	SPDAnalysisModel *SPDModel = SPDAnalysisModel::InitInstance();
	SPDModel->ParseTraceFile( this->data);
	std::cout<< "Normalizing" << std::endl;
	SPDModel->NormalizeData();
	std::cout<< "clustering" << std::endl;
	SPDModel->ClusterAgglomerate( 0.5);
	//std::cout<< "Merging" << std::endl;
	//SPDModel->ClusterMerge( 0.9, 0.9);
	std::cout<< "Generating MST" << std::endl;
	SPDModel->GenerateMST();
	vtkSmartPointer<vtkTable> table = SPDModel->GetMSTTable(0);
	if( table != NULL)
	{
		std::vector<std::string> headers;
		SPDModel->GetTableHeaders( headers);
		this->graph->SetGraphTable( table, headers[0], headers[1], headers[2]);
		this->graph->ShowGraphWindow();
	}
}

void SampleEditor::sampledendrogram()
{
	if( this->data->GetNumberOfRows() <= 0)
	{
		return;
	}

	double** datas;
	vtkVariant temp; 

	datas = new double*[this->data->GetNumberOfRows()];

	std::cout<<this->data->GetNumberOfRows()<<endl;
	std::cout<<this->data->GetNumberOfColumns()<<endl;

	for (int i = 0; i < this->data->GetNumberOfRows(); i++)
	{
		datas[i] = new double[this->data->GetNumberOfColumns() - 1 ];
	}


	for(int i = 0; i < this->data->GetNumberOfRows(); i++)
	{		
		for(int j = 1; j < this->data->GetNumberOfColumns(); j++)
		{
			temp = this->data->GetValue(i, j);
			datas[i][j-1] = temp.ToDouble();
		}
	}

	cc1 = new clusclus(datas, (int)this->data->GetNumberOfRows(), (int)this->data->GetNumberOfColumns() - 1);
	cc1->RunClusClus();
	cc1->MergersToProgress();
	cc1->PrepareTreeData();
	cc1->GetOptimalLeafOrderD();
	cc1->WriteClusteringOutputToFile("mergers.txt","features.txt","progress.txt", "members.txt",
		"gap.txt", "treedata.txt", "Optimalleaforder.txt");

	this->dendro1->setTreeData(cc1->num_samples, cc1->treedata, cc1->optimalleaforder);
	this->dendro1->createDataForDendogram();
	this->dendro1->showGraph();

	for (int i = 0; i < this->data->GetNumberOfRows(); i++)
	{
		delete datas[i];
	}
	delete datas;
}

void SampleEditor::featuredendrogram()
{
	cc1->Transpose();
	cc2 = new clusclus(cc1->transposefeatures,cc1->num_features, cc1->num_samples);
	cc2->RunClusClus();
	cc2->MergersToProgress();
	cc2->PrepareTreeData();
	cc2->GetOptimalLeafOrderD();
	cc2->WriteClusteringOutputToFile("mergers2.txt","features2.txt","progress2.txt", "members2.txt",
		"gap2.txt", "treedata2.txt", "Optimalleaforder2.txt");

	this->dendro2->setTreeData(cc2->num_samples, cc2->treedata, cc2->optimalleaforder);
	this->dendro2->createDataForDendogram();
	this->dendro2->showGraph();
}

void SampleEditor::showheatmap()
{
	this->heatmap->setDataForHeatmap(cc1->features, cc1->optimalleaforder, cc2->optimalleaforder,cc1->num_samples, cc2->num_samples);
	this->heatmap->creatDataForHeatmap();
	this->heatmap->showGraph();

}