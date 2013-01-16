// (c) 2000 Peter Putzer

#include <qdatastream.h>
#include <kdatastream.h>

#include "Data.h"

KSVData::KSVData ()
{
  mNrChanged = mLabelChanged = mFilenameChanged = mOtherChanged = mNewEntry = false;
  mNr = mOldNr = 0;
}

KSVData::KSVData (const QString& file, const QString& path,
                  const QString& label, Q_INT8 nr)
  : mPath (path),
    mLabel (label), mFilename (file)
{
  mNrChanged = mLabelChanged = mFilenameChanged = mOtherChanged = mNewEntry = false;
  mNr = mOldNr = 0;
  setNumber (nr);
  mNrChanged = false;
}

KSVData::KSVData (const KSVData& rhs)
  : mPath (rhs.mPath),
    mLabel (rhs.mLabel),
    mFilename (rhs.mFilename),
    mRunlevel (rhs.mRunlevel),
    mNr (rhs.mNr),
    mOldLabel (rhs.mOldLabel),
    mOldFilename (rhs.mOldFilename),
    mOriginalRunlevel (rhs.mOriginalRunlevel),
    mOldNr (rhs.mOldNr),
    mNrChanged (rhs.mNrChanged),
    mLabelChanged (rhs.mLabelChanged),
    mOtherChanged (rhs.mOtherChanged),
    mFilenameChanged (rhs.mFilenameChanged),
    mNewEntry (rhs.mNewEntry), mNumberString (rhs.mNumberString)
{
}

const KSVData& KSVData::operator= (const KSVData& rhs)
{
  if (this ==&rhs)
    return *this;

  mPath = rhs.mPath;
  mLabel = rhs.mLabel;
  mFilename = rhs.mFilename;
  mRunlevel = rhs.mRunlevel;
  mNr = rhs.mNr;

  mOldLabel = rhs.mOldLabel;
  mOldFilename = rhs.mOldFilename;
  mOriginalRunlevel = rhs.mOriginalRunlevel;
  mOldNr = rhs.mOldNr;

  mNrChanged = rhs.mNrChanged;
  mLabelChanged = rhs.mLabelChanged;
  mOtherChanged = rhs.mOtherChanged;
  mFilenameChanged = rhs.mFilenameChanged;
  mNewEntry = rhs.mNewEntry;

  mNumberString = rhs.mNumberString;

  return *this;
}

bool KSVData::operator== (const KSVData& rhs) const
{
  return mLabel == rhs.mLabel &&
    mPath == rhs.mPath && mFilename == rhs.mFilename;
  //   return mPath == rhs.mPath &&
  //    mLabel == rhs.mLabel &&
  //    mFilename == rhs.mFilename &&
  //    mRunlevel == rhs.mRunlevel &&
  //    mNr == rhs.mNr &&

  //    mOldLabel == rhs.mOldLabel &&
  //    mOldFilename == rhs.mOldFilename &&
  //    mOriginalRunlevel == rhs.mOriginalRunlevel &&
  //    mOldNr == rhs.mOldNr &&

  //    mNrChanged == rhs.mNrChanged &&
  //    mLabelChanged == rhs.mLabelChanged &&
  //    mOtherChanged == rhs.mOtherChanged &&
  //    mFilenameChanged == rhs.mFilenameChanged &&
  //    mNewEntry == rhs.mNewEntry;
}

bool KSVData::operator< (const KSVData& rhs) const
{
  return mNr< rhs.mNr;
}

void KSVData::setPath (const QString& s)
{
  if (mPath != s)
    mOtherChanged = true;

  mPath = s;
}

void KSVData::setLabel (const QString& s)
{
  if (!mLabelChanged && mLabel != s)
    {
      mOldLabel = mLabel;
      mLabelChanged = true;
    }

  mLabel = s;
}

void KSVData::setFilename (const QString& s)
{
  if (!mFilenameChanged && mFilename != s)
    {
      mOldFilename = mFilename;
      mFilenameChanged = true;
    }

  mFilename = s;
}

void KSVData::setRunlevel (const QString& s)
{
  mRunlevel = s;
}

void KSVData::setNumber (Q_INT8 nr)
{
  if (!mNrChanged && mNr != nr)
    {
      mOldNr = mNr;
      mNrChanged = true;
    }

  mNr = nr;
  mNumberString.sprintf ("%.2i", nr);
}

void KSVData::setChanged (bool val)
{
  mNrChanged = mLabelChanged = mOtherChanged = mFilenameChanged = val;
}

void KSVData::setOriginalRunlevel (const QString& rl)
{
  mOriginalRunlevel = rl;
}

QDataStream& operator<< (QDataStream& stream, const KSVData& data)
{
  return stream << data.mPath
                << data.mFilename
                << data.mNr
                << data.mRunlevel
                << data.mLabel
                << data.mOldFilename
                << data.mOldNr
                << data.mOldLabel
                << data.mNrChanged
                << data.mLabelChanged
                << data.mFilenameChanged
                << data.mOtherChanged << data.mOriginalRunlevel << data.mNewEntry;

}

QDataStream& operator>> (QDataStream& stream, KSVData& data)
{
  stream >> data.mPath
         >> data.mFilename
         >> data.mNr
         >> data.mRunlevel
         >> data.mLabel
         >> data.mOldFilename
         >> data.mOldNr
         >> data.mOldLabel
         >> data.mNrChanged
         >> data.mLabelChanged
         >> data.mFilenameChanged
         >> data.mOtherChanged >> data.mOriginalRunlevel >> data.mNewEntry;


  data.mNumberString.sprintf ("%.2i", data.mNr);

  return stream;
}
