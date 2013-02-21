/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


void OptionsDlgWidget::slotValueChanged()
{
    emit valueChanged ();
}


void OptionsDlgWidget::slotBrowseTapeDevice()
{
    KURL url;
    url = KFileDialog::getOpenURL( _tapeDevice->text() );

    if ( !url.isEmpty() )
    {
        if( !url.isLocalFile() )
        {
            KMessageBox::sorry( 0L, i18n( "Only local files are supported" ) );
            return;
        }
        _tapeDevice->setText( url.path() );
    }
}


void OptionsDlgWidget::slotBrowseTarCommand()
{
    KURL url;
    url = KFileDialog::getOpenURL( _tarCommand->text() );

    if ( !url.isEmpty() )
    {
        if( !url.isLocalFile() )
        {
            KMessageBox::sorry( 0L, i18n( "Only local files are currently supported" ) );
            return;
        }
        _tarCommand->setText( url.path() );
    }
}
