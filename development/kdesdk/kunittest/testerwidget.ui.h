/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/


void TesterWidget::init()
{
    connect(m_pshRun, SIGNAL(clicked()), this, SIGNAL(run()));
}

QListView * TesterWidget::resultList()
{
    return m_lvResults;
}


QTextEdit * TesterWidget::details()
{
    return m_teDetails;
}


QProgressBar * TesterWidget::progressBar()
{
    return m_pbProgress;
}


QLabel * TesterWidget::resultsLabel()
{
    return m_lbResults;
}


QComboBox * TesterWidget::selectCombo()
{
    return m_cbSelect;
}
