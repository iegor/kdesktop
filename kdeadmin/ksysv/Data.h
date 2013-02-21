// (c) 2000 Peter Putzer

#ifndef KSV_DATA_H
#define KSV_DATA_H

class QDataStream;

class KSVData
{
public:
  KSVData ();
  KSVData (const QString& file, const QString& path, const QString& label, Q_INT8 nr);
  KSVData (const KSVData& rhs);
  inline ~KSVData () {}

  const KSVData& operator= (const KSVData& rhs);
  bool operator== (const KSVData& rhs) const;
  bool operator< (const KSVData& rhs) const;

  inline QString filenameAndPath () const { return mPath + "/" + mFilename; }

  inline const QString& path () const { return mPath; }
  inline const QString& label () const { return mLabel; }
  inline const QString& filename () const { return mFilename; }
  inline const QString& runlevel () const { return mRunlevel; }
  inline Q_INT8 number () const { return mNr; }

  inline const QString& numberString () const { return mNumberString; }

  inline const QString& oldLabel () const { return mOldLabel; }
  inline const QString& oldFilename () const { return mOldFilename; }
  inline const QString& originalRunlevel () const { return mOldFilename; }
  inline Q_INT8 oldNumber () const { return mOldNr; }
  
  inline bool numberChanged () const { return mNrChanged; }
  inline bool labelChanged () const { return mLabelChanged; }
  inline bool otherChanged () const { return mOtherChanged; }
  inline bool filenameChanged () const { return mFilenameChanged; }
  inline bool newEntry () const { return mNewEntry; }

  inline bool changed () const { return mNrChanged || mLabelChanged || mFilenameChanged; }

  inline void setNumberChanged (bool val) { mNrChanged = val; }
  inline void setNewEntry (bool val) { mNewEntry = val; }
  void setPath (const QString&);
  void setLabel (const QString&);
  void setFilename (const QString&);
  void setRunlevel (const QString&);
  void setOriginalRunlevel (const QString&);
  void setNumber (Q_INT8 nr);
  void setChanged (bool);
 
private:
  friend QDataStream& operator<< (QDataStream&, const KSVData&);
  friend QDataStream& operator>> (QDataStream&, KSVData&);

  QString mPath;
  QString mLabel;
  QString mFilename;
  QString mRunlevel;
  Q_INT8 mNr;
  QString mOldLabel;
  QString mOldFilename;
  QString mOriginalRunlevel;
  Q_INT8 mOldNr;
  
  bool mNrChanged;
  bool mLabelChanged;
  bool mOtherChanged;
  bool mFilenameChanged;
  bool mNewEntry;

  QString mNumberString;
};

QDataStream& operator<< (QDataStream& stream, const KSVData& data);
QDataStream& operator>> (QDataStream& stream, KSVData& data);

#endif // KSV_DATA_H
