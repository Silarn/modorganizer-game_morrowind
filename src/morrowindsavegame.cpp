#include "morrowindsavegame.h"

#include <Windows.h>

MorrowindSaveGame::MorrowindSaveGame(QString const &fileName, MOBase::IPluginGame const *game) :
  GamebryoSaveGame(fileName, game)
{
  FileWrapper file(this, "TES3");
  //file.skip<unsigned long>(); // header size
  //file.skip<unsigned long>(); // header version
  file.skip<unsigned long>(79); //Mostly empty header Data
  //file.readPlugins(); normal readPlugins function does not work
  file.skip<unsigned char>();
  uint8_t count;
  file.read(count);
  this->m_Plugins.reserve(count);
  std::vector<char> buffer(255);
  file.skip<unsigned short>();
  file.read(buffer.data(), 4);
  while(QString::fromLatin1(buffer.data(), 4)=="MAST"){
	  uint8_t len;
	  file.read(len);
	  file.skip<unsigned char>(3);
      QString name;
      file.read(buffer.data(), len-1);
	  name=QString::fromLatin1(buffer.data(), len-1);
	  file.skip<unsigned char>();
	  file.skip<unsigned long>(4);
	  file.read(buffer.data(), 4);
	  this->m_Plugins.push_back(name);
  }
  
  file.skip<unsigned long>(7);
  file.read(buffer.data(), 64);
  m_PCLocation=QString::fromLatin1(buffer.data(), 64).trimmed();
  
  file.skip<unsigned long>();
  file.read(buffer.data(), 32);
  
  m_PCName=QString::fromLatin1(buffer.data(), 32).trimmed();
  
  file.skip<unsigned char>(36);
  
  file.readImage(128, 128, 0, 1);
  
  this->m_Screenshot=this->m_Screenshot.invertPixels(QImage::InvertRgba); //not sure about this one
  
  //definitively have to use another method to access the player level
  //it is stored in the fifth byte of the NPDT subrecord of the first NPC_ record
  
  m_PCLevel=1; //Placeholder
  
  //file.skip<unsigned long>(8); //Record SCRD
  //file.skip<unsigned long>(16385); //Record SCRS
  //file.skip<unsigned char>(8445); //Globals
  
  //Globals, Scripts, Regions
  //file.skip<unsigned char>();
  std::vector<char> buff(4);
  file.read(buff.data(), 4);
  while(QString::fromLatin1(buff.data(), 4)=="GLOB"||QString::fromLatin1(buff.data(), 4)=="SCPT"||QString::fromLatin1(buff.data(), 4)=="REGN")
  {
    uint8_t len;
	file.read(len);
	file.skip<unsigned char>(11+len);
	file.read(buff.data(), 4);
  }
  
  file.skip<unsigned long>(4);
  
  file.read(buff.data(), 4);
  while(QString::fromLatin1(buff.data(), 4)=="NAME"||QString::fromLatin1(buff.data(), 4)=="FNAME"||QString::fromLatin1(buff.data(), 4)=="RNAM"||QString::fromLatin1(buff.data(), 4)=="CNAM"||QString::fromLatin1(buff.data(), 4)=="ANAM"||QString::fromLatin1(buff.data(), 4)=="BNAM"||QString::fromLatin1(buff.data(), 4)=="KNAM")
  {
    uint8_t len;
	file.read(len);
	file.skip<unsigned char>(3+len);
	file.read(buff.data(), 4);
  }
  
  file.skip<unsigned char>(7);
  file.read(m_PCLevel); 
  
  m_SaveNumber=fileName.chopped(4).right(4).toInt();
}
