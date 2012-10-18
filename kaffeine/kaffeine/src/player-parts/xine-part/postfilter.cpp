/*
 * postfilter.cpp - wrapper for xine's postprocessing filters
 *
 * Copyright (C) 2003-2005 Jürgen Kofler <kaffeine@gmx.net>
 * Copyright (C) 2003-2005 Miguel Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <kpushbutton.h>
#include <kseparator.h>
#include <klocale.h>
#include <kdebug.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qobject.h>
#include <qstring.h>
#include <qgroupbox.h>
#include <qtextedit.h>

#include <stdio.h>

#include "postfilter.h"

PostFilter::PostFilter(const QString& name, xine_t* engine, xine_audio_port_t* audioDriver,
              xine_video_port_t* videoDriver, QWidget *parent) : QObject(parent), m_data(NULL), m_groupBox(NULL)
{
  m_filterName = name;
  kdDebug() << "PostFilter: Create Postprocessing Filter: " << m_filterName << endl;

  m_xineEngine = engine;

  m_xinePost = xine_post_init(m_xineEngine, m_filterName.ascii(), 0, &audioDriver, &videoDriver );
  if(m_xinePost)
  {
    xine_post_in_t* inputAPI = NULL;

    m_groupBox = new QGroupBox(name, parent);
    m_groupBox->setSizePolicy(QSizePolicy (QSizePolicy::Minimum, QSizePolicy::Fixed));
    QGridLayout* grid = new QGridLayout(m_groupBox, 2, 2);
    grid->setMargin( 20 );
    grid->setSpacing( 5 );
    int row = 0;

    if ((inputAPI = (xine_post_in_t*)xine_post_input(m_xinePost, const_cast<char*>("parameters"))))
    {
      m_xinePostAPI = (xine_post_api_t*)inputAPI->data;
      m_xinePostDescr = m_xinePostAPI->get_param_descr();
      m_xinePostParameter = m_xinePostDescr->parameter;

      m_data = new char[m_xinePostDescr->struct_size];
      m_xinePostAPI->get_parameters(m_xinePost, m_data);

      QLabel* descr;

      while (m_xinePostParameter->type != POST_PARAM_TYPE_LAST)
      {
        kdDebug() << "PostFilter: Parameter: " << m_xinePostParameter->name << endl;
        if (m_xinePostParameter->readonly) continue;

        switch (m_xinePostParameter->type)
        {
          case POST_PARAM_TYPE_INT:
          {
            if (m_xinePostParameter->enum_values)
            {
              PostFilterParameterCombo* parameter = new
                PostFilterParameterCombo(m_xinePostParameter->name, m_xinePostParameter->offset,
                                          *(int*)(m_data+m_xinePostParameter->offset),
                                          m_xinePostParameter->enum_values, m_groupBox );
              connect(parameter, SIGNAL(signalIntValue(int, int)), this, SLOT(slotApplyIntValue(int, int)));
              m_parameterList.append(parameter);

              grid->addWidget(parameter->getWidget(), row, 0);
            }
            else
            {
              PostFilterParameterInt* parameter = new
                PostFilterParameterInt(m_xinePostParameter->name, m_xinePostParameter->offset,
                                        *(int*)(m_data+m_xinePostParameter->offset),
                                        (int)m_xinePostParameter->range_min, (int)m_xinePostParameter->range_max,
                                        m_groupBox);

              connect(parameter, SIGNAL( signalIntValue(int, int)), this, SLOT( slotApplyIntValue(int, int)));
              m_parameterList.append(parameter);

              grid->addWidget(parameter->getWidget(), row, 0);
            }
            break;
          }
          case POST_PARAM_TYPE_DOUBLE:
          {
            PostFilterParameterDouble* parameter = new
              PostFilterParameterDouble(m_xinePostParameter->name, m_xinePostParameter->offset,
                                         *(double*)(m_data+m_xinePostParameter->offset),
                                         (double)m_xinePostParameter->range_min, (double)m_xinePostParameter->range_max,
                                         m_groupBox);

            connect(parameter, SIGNAL(signalDoubleValue(int, double)), this, SLOT(slotApplyDoubleValue(int, double)));
            m_parameterList.append(parameter);

            grid->addWidget(parameter->getWidget(), row, 0);
            break;
          }
          case POST_PARAM_TYPE_CHAR:
          {
            PostFilterParameterChar* parameter = new
              PostFilterParameterChar(m_xinePostParameter->name, m_xinePostParameter->offset,
                                       (char*)(m_data+m_xinePostParameter->offset), m_xinePostParameter->size,
                                       m_groupBox);

            connect( parameter, SIGNAL(signalCharValue(int, const QString&)), this, SLOT(slotApplyCharValue(int, const QString&)));
            m_parameterList.append(parameter);

            grid->addWidget(parameter->getWidget(), row, 0);
            break;
          }
          case POST_PARAM_TYPE_STRING:
          case POST_PARAM_TYPE_STRINGLIST: break; /* not implemented */
          case POST_PARAM_TYPE_BOOL:
          {
            PostFilterParameterBool* parameter = new
              PostFilterParameterBool(m_xinePostParameter->name, m_xinePostParameter->offset,
                                       (bool) *(int*)(m_data+m_xinePostParameter->offset), m_groupBox);

            connect(parameter, SIGNAL(signalIntValue(int, int)), this, SLOT(slotApplyIntValue(int, int)));
            m_parameterList.append(parameter);

            grid->addWidget(parameter->getWidget(), row, 0);
            break;
          }
          default: break;
        }

        descr = new QLabel(QString::fromUtf8(m_xinePostParameter->description ), m_groupBox);
        descr->setAlignment(QLabel::WordBreak | QLabel::AlignVCenter);
        grid->addWidget(descr, row, 1);
        row++;
        m_xinePostParameter++;
      }
    }
    KSeparator* sep = new KSeparator(KSeparator::Horizontal, m_groupBox);
    grid->addMultiCellWidget(sep, row, row, 0, 1);
    row++;
    KPushButton* deleteButton = new KPushButton(i18n("Delete Filter"), m_groupBox);
    deleteButton->setSizePolicy (QSizePolicy (QSizePolicy::Minimum, QSizePolicy::Fixed));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT( slotDeletePressed()));
    grid->addWidget(deleteButton, row, 0);

    if(inputAPI)
    {
      KPushButton* helpButton = new KPushButton(i18n("Help"), m_groupBox);
      helpButton->setSizePolicy(QSizePolicy (QSizePolicy::Minimum, QSizePolicy::Fixed));
      connect(helpButton, SIGNAL(clicked()), this, SLOT(slotHelpPressed()));
      grid->addWidget(helpButton, row, 1);
    }

    if (parent)
      m_groupBox->show();
  }
}

PostFilter::~PostFilter()
{
  kdDebug() << "PostFilter: Delete Postprocessing Filter: " << m_filterName << endl;
  if(m_xinePost)
  {
    delete m_groupBox;
    delete [] m_data;
    xine_post_dispose(m_xineEngine, m_xinePost);
  }
}

void PostFilter::slotApplyIntValue(int offset, int val)
{
  kdDebug() << "PostFilter: " << m_filterName << " Apply integer value " << val << " on offset " << offset << endl;
  *(int*)(m_data+offset) = val;
  m_xinePostAPI->set_parameters(m_xinePost, m_data);
}

void PostFilter::slotApplyDoubleValue(int offset, double val)
{
  kdDebug() << "PostFilter: " << m_filterName << " Apply double value " << val << " on offset " << offset << endl;
  *(double*)(m_data+offset) = val;
  m_xinePostAPI->set_parameters(m_xinePost, m_data);
}

void PostFilter::slotApplyCharValue(int offset, const QString& val)
{
  kdDebug() << "PostFilter: " << m_filterName << " Apply char value '" << val << "' on offset " << offset << endl;
  sprintf((char *)(m_data+offset), "%s", val.latin1());
  m_xinePostAPI->set_parameters(m_xinePost, m_data);
}

xine_post_in_t* PostFilter::getInput() const
{
  xine_post_in_t* input = NULL;

  kdDebug() << "PostFilter: Get input" << endl;
  if(m_xinePost)
  {
    /* look for known input ports */
    input = xine_post_input(m_xinePost, const_cast<char*>("video"));
    if( !input )
      input = xine_post_input(m_xinePost, const_cast<char*>("video in") );
    if( !input )
      input = xine_post_input(m_xinePost, const_cast<char*>("audio"));
    if( !input )
      input = xine_post_input(m_xinePost, const_cast<char*>("audio in"));
  }
  return input;
}


xine_post_out_t* PostFilter::getOutput() const
{
  xine_post_out_t* output = NULL;

  kdDebug() << "PostFilter: Get output" << endl;

  if(m_xinePost)
  {
    /* look for known output ports */
    output = xine_post_output(m_xinePost, const_cast<char*>("video"));
    if( !output )
      output = xine_post_output(m_xinePost, const_cast<char*>("video out"));
    if( !output )
      output = xine_post_output(m_xinePost, const_cast<char*>("audio"));
    if( !output )
      output = xine_post_output(m_xinePost, const_cast<char*>("audio out"));

    if(!output)
    {
      /* fallback to the first available output port.
       * some video plugins have funky port names :)
       */
      const char *const *outs = xine_post_list_outputs(m_xinePost);
      return (xine_post_out_t*)xine_post_output(m_xinePost, (char *) *outs);
    }
  }

  return output;
}


void PostFilter::slotHelpPressed()
{
  kdDebug() << "PostFilter: Help pressed" << endl;

  PostFilterHelp* filterHelp = new PostFilterHelp(NULL, m_filterName.ascii(), QString::fromUtf8(m_xinePostAPI->get_help()));
  filterHelp->exec();

  delete filterHelp;
}


QString PostFilter::getConfig()
{
 /*
  *  returns a string like "filtername:parameter=value,parameter=value,..."
  */

  QString configString;
  QTextOStream configStream(&configString);

  configStream << m_filterName << ":";
  for (uint i = 0; i < m_parameterList.count(); i++)
  {
    configStream << m_parameterList.at( i )->name() << "=" << m_parameterList.at( i )->getValue();
    if( i != m_parameterList.count()-1 )
      configStream << ",";
  }

  kdDebug() << "PostFilter: GetConfig " << configString << endl;

  return configString;
}

void PostFilter::setConfig(const QString &configString)
{
 /*
  *  expects a string like filtername:parameter=value,parameter=value,...
  *  or filtername:parameter="value",parameter="value",...
  */

  kdDebug() << "PostFilter: SetConfig " << configString << endl;

  QString configStr;
  if (configString.section(':',0,0) == m_filterName)
  {
    configStr = configString.section(':',1,1);
  }
  else
  {
    kdWarning() << "PostFilter: Config string doesn't match filter name " << m_filterName << endl;
    kdDebug() << "PostFilter: Don't apply new configuration" << endl;
    return;
  }

  for( int i = 0; i < configStr.contains(',') + 1; i++ )
  {
    QString parameterConfig = configStr.section(',', i, i);
    QString parameterName = parameterConfig.section('=', 0, 0);
    QString parameterValue = parameterConfig.section('=', 1, 1);
    parameterValue = parameterValue.remove('"');

    for (uint j = 0; j < m_parameterList.count(); j++)
    {
      if(parameterName == m_parameterList.at(j)->name())
      {
        kdDebug() << "PostFilter: Set parameter '" << parameterName << "' to value '" << parameterValue << "'" << endl;
	m_parameterList.at(j)->setValue(parameterValue);
      }
    }
  }
}

PostFilterParameterInt::PostFilterParameterInt(const QString& name, int offset, int value, int min, int max, QWidget* parent)
  : PostFilterParameter (name, offset, parent )
{
  m_numInput = new KIntNumInput(value, parent);
  m_numInput->setRange( min, max, 1, false);
  connect(m_numInput, SIGNAL(valueChanged(int)), this, SLOT(slotIntValue(int)));
}

PostFilterParameterDouble::PostFilterParameterDouble(const QString& name, int offset, double value, double min, double max, QWidget* parent)
  : PostFilterParameter (name, offset, parent )
{
  m_numInput = new KDoubleNumInput(parent);
  m_numInput->setValue(value);
  m_numInput->setRange(min, max, 0.01, false);
  connect(m_numInput, SIGNAL(valueChanged( double)), this, SLOT(slotDoubleValue( double)));
}

PostFilterParameterChar::PostFilterParameterChar(const QString& name, int offset, char *value, int size, QWidget* parent)
  : PostFilterParameter (name, offset, parent )
{
  m_charInput = new KLineEdit(value, parent);
  m_charInput->setMaxLength(size);
  connect(m_charInput, SIGNAL(returnPressed(const QString&)), this, SLOT(slotCharValue(const QString&)));
}

PostFilterParameterCombo::PostFilterParameterCombo(const QString& name, int offset, int value, char **enums, QWidget* parent)
  : PostFilterParameter (name, offset, parent)
{
  m_comboBox = new KComboBox(parent);
  for (int i = 0; enums[i]; i++)
  {
    m_comboBox->insertItem(enums[i]);
  }
  m_comboBox->setCurrentItem(value);
  connect(m_comboBox, SIGNAL( activated(int)), this, SLOT( slotIntValue(int)));
}

PostFilterParameterBool::PostFilterParameterBool(const QString& name, int offset, bool value, QWidget* parent)
  : PostFilterParameter (name, offset, parent )
{
  m_checkBox = new QCheckBox(parent);
  m_checkBox->setChecked(value);
  connect(m_checkBox, SIGNAL(toggled(bool)), this, SLOT(slotBoolValue( bool)));
}

PostFilterHelp::PostFilterHelp(QWidget *parent, const char *name, const QString& text)
 : KDialogBase( parent, name, true, QString(name) + " - " + i18n("Help"), KDialogBase::Close )
{
  setInitialSize( QSize(500,500) );

  QWidget* mainWidget = makeMainWidget();
  QGridLayout* grid = new QGridLayout( mainWidget, 1, 1 );
  grid->setSpacing( 5 );

  //QString help = QString::fromUtf8(text);
  m_textEdit = new QTextEdit(text, QString::null, mainWidget, name);
  m_textEdit->setReadOnly(true);
  grid->addWidget(m_textEdit, 0, 0);
}


PostFilterHelp::~PostFilterHelp()
{
  delete m_textEdit;
}

#include "postfilter.moc"
