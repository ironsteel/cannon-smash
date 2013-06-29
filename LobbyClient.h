/* $Id: LobbyClient.h,v 1.20 2003/07/29 16:52:15 nan Exp $ */

// Copyright (C) 2001-2003  ¿ÀÆî µÈ¹¨(Kanna Yoshihiro)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef _LobbyClient_
#define _LobbyClient_

#include <gtk/gtk.h>

#define LOBBYSERVER_NAME	"nan.p.utmc.or.jp"
#define LOBBYSERVER_PORT	(5735)

class PlayerInfo;
class LobbyClientView;

static struct {
  char *code;
  char *langname;
  int langID;
} table[] = {
  { "aa", _("Afar"), 0x00 },
  { "ab", _("Abkhazian"), 0x00 },
  { "ae", _("Avestan"), 0x00 },
  { "af", _("Afrikaans"), 0x36 },
  { "am", _("Amharic"), 0x00 },
  { "ar", _("Arabic"), 0x01 },
  { "as", _("Assamese"), 0x4d },
  { "ay", _("Aymara"), 0x00 },
  { "az", _("Azerbaijani"), 0x2c },
  { "ba", _("Bashkir"), 0x00 },
  { "be", _("Belarusian"), 0x23 },
  { "bg", _("Bulgarian"), 0x02 },
  { "bh", _("Bihari"), 0x00 },
  { "bi", _("Bislama"), 0x00 },
  { "bn", _("Bengali"), 0x45 },
  { "bo", _("Tibetan"), 0x00 },
  { "br", _("Breton"), 0x00 },
  { "bs", _("Bosnian"), 0x00 },
  { "ca", _("Catalan"), 0x03 },
  { "ce", _("Chechen"), 0x00 },
  { "ch", _("Chamorro"), 0x00 },
  { "co", _("Corsican"), 0x00 },
  { "cs", _("Czech"), 0x05 },
  { "cu", _("Church Slavic"), 0x00 },
  { "cv", _("Chuvash"), 0x00 },
  { "cy", _("Welsh"), 0x00 },
  { "da", _("Danish"), 0x06 },
  { "de", _("German"), 0x07 },
  { "dz", _("Dzongkha"), 0x00 },
  { "el", _("Greek"), 0x08 },
  { "en", _("English"), 0x09 },
  { "eo", _("Esperanto"), 0x00 },
  { "es", _("Spanish"), 0x0a },
  { "et", _("Estonian"), 0x25 },
  { "eu", _("Basque"), 0x2d },
  { "fa", _("Persian"), 0x29 },
  { "fi", _("Finnish"), 0x0b },
  { "fj", _("Fijian"), 0x00 },
  { "fo", _("Faroese"), 0x38 },
  { "fr", _("French"), 0x0c },
  { "fy", _("Frisian"), 0x00 },
  { "ga", _("Irish"), 0x00 },
  { "gd", _("Scots"), 0x00 },
  { "gl", _("Galician"), 0x56 },
  { "gn", _("Guarani"), 0x00 },
  { "gu", _("Gujarati"), 0x47 },
  { "gv", _("Manx"), 0x00 },
  { "ha", _("Hausa"), 0x00 },
  { "he", _("Hebrew"), 0x0d },
  { "hi", _("Hindi"), 0x39 },
  { "ho", _("Hiri Motu"), 0x00 },
  { "hr", _("Croatian"), 0x1a },
  { "hu", _("Hungarian"), 0x0e },
  { "hy", _("Armenian"), 0x2b },
  { "hz", _("Herero"), 0x00 },
  { "ia", _("Interlingua"), 0x00 },
  { "id", _("Indonesian"), 0x21 },
  { "ie", _("Interlingue"), 0x00 },
  { "ik", _("Inupiak"), 0x00 },
  { "is", _("Icelandic"), 0x0f },
  { "it", _("Italian"), 0x10 },
  { "iu", _("Inuktitut"), 0x00 },
  { "ja", _("Japanese"), 0x11 },
  { "jw", _("Javanese"), 0x00 },
  { "ka", _("Georgian"), 0x37 },
  { "ki", _("Kikuyu"), 0x00 },
  { "kj", _("Kuanyama"), 0x00 },
  { "kk", _("Kazakh"), 0x3f },
  { "kl", _("Kalaallisut"), 0x00 },
  { "km", _("Khmer"), 0x00 },
  { "kn", _("Kannada"), 0x4b },
  { "ko", _("Korean"), 0x12 },
  { "ks", _("Kashmiri"), 0x60 },
  { "ku", _("Kurdish"), 0x00 },
  { "kv", _("Komi"), 0x00 },
  { "kw", _("Cornish"), 0x00 },
  { "ky", _("Kirghiz"), 0x40 },
  { "kok", _("Konkani"), 0x57 },
  { "la", _("Latin"), 0x00 },
  { "lb", _("Letzeburgesch"), 0x00 },
  { "ln", _("Lingala"), 0x00 },
  { "lo", _("Laotian"), 0x00 },
  { "lt", _("Lithuanian"), 0x27 },
  { "lv", _("Latvian"), 0x26 },
  { "mg", _("Malagasy"), 0x00 },
  { "mh", _("Marshall"), 0x00 },
  { "mi", _("Maori"), 0x00 },
  { "mk", _("Macedonian"), 0x2f },
  { "ml", _("Malayalam"), 0x4c },
  { "mn", _("Mongolian"), 0x50 },
  { "mo", _("Moldavian"), 0x00 },
  { "mr", _("Marathi"), 0x4e },
  { "ms", _("Malay"), 0x3e },
  { "mt", _("Maltese"), 0x00 },
  { "my", _("Burmese"), 0x00 },
  { "mni", _("Manipuri"), 0x58 },
  { "na", _("Nauru"), 0x00 },
  { "nb", _("Norwegian Bokmal"), 0x00 },
  { "nd", _("North Ndebele"), 0x00 },
  { "ne", _("Nepali"), 0x61 },
  { "ng", _("Ndonga"), 0x00 },
  { "nl", _("Dutch"), 0x13 },
  { "nn", _("Norwegian Nynorsk"), 0x00 },
  { "no", _("Norwegian"), 0x14 },
  { "nr", _("South Ndebele"), 0x00 },
  { "nv", _("Navajo"), 0x00 },
  { "ny", _("Nyanja"), 0x00 },
  { "oc", _("Occitan"), 0x00 },
  { "om", _("(Afan) Oromo"), 0x00 },
  { "or", _("Oriya"), 0x48 },
  { "os", _("Ossetian"), 0x00 },
  { "pa", _("Punjabi"), 0x46 },
  { "pi", _("Pali"), 0x00 },
  { "pl", _("Polish"), 0x15 },
  { "ps", _("Pashto"), 0x00 },
  { "pt", _("Portuguese"), 0x16 },
  { "qu", _("Quechua"), 0x00 },
  { "rm", _("Rhaeto-Roman"), 0x00 },
  { "rn", _("Kirundi"), 0x00 },
  { "ro", _("Romanian"), 0x18 },
  { "ru", _("Russian"), 0x19 },
  { "rw", _("Kinyarwanda"), 0x00 },
  { "sa", _("Sanskrit"), 0x4f },
  { "sc", _("Sardinian"), 0x00 },
  { "sd", _("Sindhi"), 0x59 },
  { "se", _("Northern Sami"), 0x00 },
  { "sg", _("Sango"), 0x00 },
  { "si", _("Sinhalese"), 0x00 },
  { "sk", _("Slovak"), 0x1b },
  { "sl", _("Slovenian"), 0x24 },
  { "sm", _("Samoan"), 0x00 },
  { "sn", _("Shona"), 0x00 },
  { "so", _("Somali"), 0x00 },
  { "sq", _("Albanian"), 0x1c },
  { "sr", _("Serbian"), 0x1a },
  { "ss", _("Siswati"), 0x00 },
  { "st", _("Sesotho"), 0x00 },
  { "su", _("Sundanese"), 0x00 },
  { "sv", _("Swedish"), 0x1d },
  { "sw", _("Swahili"), 0x41 },
  { "ta", _("Tamil"), 0x49 },
  { "te", _("Telugu"), 0x0a },
  { "tg", _("Tajik"), 0x00 },
  { "th", _("Thai"), 0x1e },
  { "ti", _("Tigrinya"), 0x00 },
  { "tk", _("Turkmen"), 0x00 },
  { "tl", _("Tagalog"), 0x00 },
  { "tn", _("Setswana"), 0x00 },
  { "to", _("Tonga"), 0x00 },
  { "tr", _("Turkish"), 0x1f },
  { "ts", _("Tsonga"), 0x00 },
  { "tt", _("Tatar"), 0x44 },
  { "tw", _("Twi"), 0x00 },
  { "ty", _("Tahitian"), 0x00 },
  { "ug", _("Uighur"), 0x00 },
  { "uk", _("Ukrainian"), 0x22 },
  { "ur", _("Urdu"), 0x20 },
  { "uz", _("Uzbek"), 0x43 },
  { "vi", _("Vietnamese"), 0x2a },
  { "vo", _("Volapuk"), 0x00 },
  { "wo", _("Wolof"), 0x00 },
  { "wen", _("Sorbian"), 0x00 },
  { "xh", _("Xhosa"), 0x00 },
  { "yi", _("Yiddish"), 0x00 },
  { "yo", _("Yoruba"), 0x00 },
  { "za", _("Zhuang"), 0x00 },
  { "zh", _("Chinese"), 0x04 },
  { "zu", _("Zulu"), 0x00 }, 
  { "", "", -1 }
};

class LobbyClient {
public:
  ~LobbyClient();

  // A sort of singleton
  static LobbyClient* Create();
  static LobbyClient* TheLobbyClient() { return m_lobbyClient; };

  bool Init( char *nickname, char *message );

  int GetSocket() { return m_socket; };
  PlayerInfo *GetPlayerInfo() { return m_player; };
  long GetPlayerNum() { return m_playerNum; };

  static gint PollServerMessage( gpointer data );
  static void Connect( GtkWidget *widget, gpointer data );

  void SendAP( long uniqID );
  void SendSP();
  void SendQP();
  void SendDP( long uniqID);
  void SendQT();
  void SendSC( int score1, int score2 );
  void SendMS( char *message, long channel );

  long m_playerNum;
  PlayerInfo *m_player;

  long m_selected;		// Selected row of the table

  char m_nickname[32];

  long GetLang() { return m_lang; };

  bool GetCanBeServer() { return m_canBeServer; };
protected:
  void ReadUI();
  void UpdateTable();

  void ReadPI();
  void ReadOI();
  void ReadOV();
  void ReadMS();

  LobbyClientView *m_view;

  int m_socket;
  bool m_canBeServer;

  long m_lang;

private:
  LobbyClient();
  static LobbyClient *m_lobbyClient;
};

class PlayerInfo {
public:
  PlayerInfo();
  ~PlayerInfo();

  bool m_canBeServer;
  bool m_playing;
  long m_ID;
  char m_nickname[32];
  char m_message[64];
};

#endif // _LobbyClient_
