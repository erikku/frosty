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

#include "KanaConversion.h"

#include <QtCore/QRegExp>
#include <QtCore/QStringList>

QStringList basicHiragana, youonHiragana, basicKatakana, youonKatakana, modernKatakana;
QStringList basicRomaji, youonRomaji, modernRomaji, specialKana, specialRomaji;
QStringList smallKana, smallRomaji;
bool tablesLoaded = false;

void setupConversionTables()
{
	if(tablesLoaded)
		return;

	basicHiragana << QString::fromUtf8("あ") << QString::fromUtf8("い") << QString::fromUtf8("う")
		<< QString::fromUtf8("え") << QString::fromUtf8("お") << QString::fromUtf8("か")
		<< QString::fromUtf8("き") << QString::fromUtf8("く") << QString::fromUtf8("け")
		<< QString::fromUtf8("こ") << QString::fromUtf8("さ") << QString::fromUtf8("し")
		<< QString::fromUtf8("す") << QString::fromUtf8("せ") << QString::fromUtf8("そ")
		<< QString::fromUtf8("た") << QString::fromUtf8("ち") << QString::fromUtf8("つ")
		<< QString::fromUtf8("て") << QString::fromUtf8("と") << QString::fromUtf8("な")
		<< QString::fromUtf8("に") << QString::fromUtf8("ぬ") << QString::fromUtf8("ね")
		<< QString::fromUtf8("の") << QString::fromUtf8("は") << QString::fromUtf8("ひ")
		<< QString::fromUtf8("ふ") << QString::fromUtf8("へ") << QString::fromUtf8("ほ")
		<< QString::fromUtf8("ま") << QString::fromUtf8("み") << QString::fromUtf8("む")
		<< QString::fromUtf8("め") << QString::fromUtf8("も") << QString::fromUtf8("や")
		<< QString::fromUtf8("ゆ") << QString::fromUtf8("よ") << QString::fromUtf8("ら")
		<< QString::fromUtf8("り") << QString::fromUtf8("る") << QString::fromUtf8("れ")
		<< QString::fromUtf8("ろ") << QString::fromUtf8("わ") // << QString::fromUtf8("ん")
		<< QString::fromUtf8("ゐ") << QString::fromUtf8("ゑ")
		<< QString::fromUtf8("うぃ") << QString::fromUtf8("うぇ") << QString::fromUtf8("を")
		<< QString::fromUtf8("が") << QString::fromUtf8("ぎ") << QString::fromUtf8("ぐ")
		<< QString::fromUtf8("げ") << QString::fromUtf8("ご") << QString::fromUtf8("ざ")
		<< QString::fromUtf8("ぢ") << QString::fromUtf8("づ") << QString::fromUtf8("ぜ")
		<< QString::fromUtf8("ぢ") << QString::fromUtf8("づ")
		<< QString::fromUtf8("ぞ") << QString::fromUtf8("だ") << QString::fromUtf8("じ")
		<< QString::fromUtf8("ず") << QString::fromUtf8("で") << QString::fromUtf8("ど")
		<< QString::fromUtf8("ば") << QString::fromUtf8("び") << QString::fromUtf8("ぶ")
		<< QString::fromUtf8("べ") << QString::fromUtf8("ぼ") << QString::fromUtf8("ぱ")
		<< QString::fromUtf8("ぴ") << QString::fromUtf8("ぷ") << QString::fromUtf8("ぺ")
		<< QString::fromUtf8("ぽ");

	youonHiragana << QString::fromUtf8("きゃ") << QString::fromUtf8("きゅ") << QString::fromUtf8("きょ")
		<< QString::fromUtf8("しゃ") << QString::fromUtf8("しゅ") << QString::fromUtf8("しょ")
		<< QString::fromUtf8("ちゃ") << QString::fromUtf8("ちゅ") << QString::fromUtf8("ちょ")
		<< QString::fromUtf8("にゃ") << QString::fromUtf8("にゅ") << QString::fromUtf8("にょ")
		<< QString::fromUtf8("ひゃ") << QString::fromUtf8("ひゅ") << QString::fromUtf8("ひょ")
		<< QString::fromUtf8("みゃ") << QString::fromUtf8("みゅ") << QString::fromUtf8("みょ")
		<< QString::fromUtf8("りゃ") << QString::fromUtf8("りゅ") << QString::fromUtf8("りょ")
		<< QString::fromUtf8("ぎゃ") << QString::fromUtf8("ぎゅ") << QString::fromUtf8("ぎょ")
		<< QString::fromUtf8("ぢゃ") << QString::fromUtf8("ぢゅ") << QString::fromUtf8("ぢょ")
		<< QString::fromUtf8("じゃ") << QString::fromUtf8("じゅ") << QString::fromUtf8("じょ")
		<< QString::fromUtf8("びゃ") << QString::fromUtf8("びゅ") << QString::fromUtf8("びょ")
		<< QString::fromUtf8("ぴゃ") << QString::fromUtf8("ぴゅ") << QString::fromUtf8("ぴょ");

	basicKatakana << QString::fromUtf8("ア") << QString::fromUtf8("イ") << QString::fromUtf8("ウ")
		<< QString::fromUtf8("エ") << QString::fromUtf8("オ") << QString::fromUtf8("カ")
		<< QString::fromUtf8("キ") << QString::fromUtf8("ク") << QString::fromUtf8("ケ")
		<< QString::fromUtf8("コ") << QString::fromUtf8("サ") << QString::fromUtf8("シ")
		<< QString::fromUtf8("ス") << QString::fromUtf8("セ") << QString::fromUtf8("ソ")
		<< QString::fromUtf8("タ") << QString::fromUtf8("チ") << QString::fromUtf8("ツ")
		<< QString::fromUtf8("テ") << QString::fromUtf8("ト") << QString::fromUtf8("ナ")
		<< QString::fromUtf8("ニ") << QString::fromUtf8("ヌ") << QString::fromUtf8("ネ")
		<< QString::fromUtf8("ノ") << QString::fromUtf8("ハ") << QString::fromUtf8("ヒ")
		<< QString::fromUtf8("フ") << QString::fromUtf8("ヘ") << QString::fromUtf8("ホ")
		<< QString::fromUtf8("マ") << QString::fromUtf8("ミ") << QString::fromUtf8("ム")
		<< QString::fromUtf8("メ") << QString::fromUtf8("モ") << QString::fromUtf8("ヤ")
		<< QString::fromUtf8("ユ") << QString::fromUtf8("ヨ") << QString::fromUtf8("ラ")
		<< QString::fromUtf8("リ") << QString::fromUtf8("ル") << QString::fromUtf8("レ")
		<< QString::fromUtf8("ロ") << QString::fromUtf8("ワ") // QString::fromUtf8("ン")
		<< QString::fromUtf8("ヰ") << QString::fromUtf8("ヱ")
		<< QString::fromUtf8("ウィ") << QString::fromUtf8("ウェ") << QString::fromUtf8("ヲ")
		<< QString::fromUtf8("ガ") << QString::fromUtf8("ギ") << QString::fromUtf8("グ")
		<< QString::fromUtf8("ゲ") << QString::fromUtf8("ゴ") << QString::fromUtf8("ザ")
		<< QString::fromUtf8("ヂ") << QString::fromUtf8("ヅ") << QString::fromUtf8("ゼ")
		<< QString::fromUtf8("ヂ") << QString::fromUtf8("ヅ")
		<< QString::fromUtf8("ゾ") << QString::fromUtf8("ダ") << QString::fromUtf8("ジ")
		<< QString::fromUtf8("ズ") << QString::fromUtf8("デ") << QString::fromUtf8("ド")
		<< QString::fromUtf8("バ") << QString::fromUtf8("ビ") << QString::fromUtf8("ブ")
		<< QString::fromUtf8("ベ") << QString::fromUtf8("ボ") << QString::fromUtf8("パ")
		<< QString::fromUtf8("ピ") << QString::fromUtf8("プ") << QString::fromUtf8("ペ")
		<< QString::fromUtf8("ポ");

	youonKatakana << QString::fromUtf8("キャ") << QString::fromUtf8("キュ") << QString::fromUtf8("キョ")
		<< QString::fromUtf8("シャ") << QString::fromUtf8("シュ") << QString::fromUtf8("ショ")
		<< QString::fromUtf8("チャ") << QString::fromUtf8("チュ") << QString::fromUtf8("チョ")
		<< QString::fromUtf8("ニャ") << QString::fromUtf8("ニュ") << QString::fromUtf8("ニョ")
		<< QString::fromUtf8("ヒャ") << QString::fromUtf8("ヒュ") << QString::fromUtf8("ヒョ")
		<< QString::fromUtf8("ミャ") << QString::fromUtf8("ミュ") << QString::fromUtf8("ミョ")
		<< QString::fromUtf8("リャ") << QString::fromUtf8("リュ") << QString::fromUtf8("リョ")
		<< QString::fromUtf8("ギャ") << QString::fromUtf8("ギュ") << QString::fromUtf8("ギョ")
		<< QString::fromUtf8("ヂャ") << QString::fromUtf8("ヂュ") << QString::fromUtf8("ヂョ")
		<< QString::fromUtf8("ジャ") << QString::fromUtf8("ジュ") << QString::fromUtf8("ジョ")
		<< QString::fromUtf8("ビャ") << QString::fromUtf8("ビュ") << QString::fromUtf8("ビョ")
		<< QString::fromUtf8("ピャ") << QString::fromUtf8("ピュ") << QString::fromUtf8("ピョ");

	modernKatakana << QString::fromUtf8("イェ") << QString::fromUtf8("ウィ") << QString::fromUtf8("ウェ")
		<< QString::fromUtf8("ウォ") << QString::fromUtf8("ヷ") << QString::fromUtf8("ヸ")
		<< QString::fromUtf8("ヴ") << QString::fromUtf8("ヹ") << QString::fromUtf8("ヺ")
		<< QString::fromUtf8("ヴァ") << QString::fromUtf8("ヴィ") << QString::fromUtf8("ヴェ")
		<< QString::fromUtf8("ヴォ") << QString::fromUtf8("シェ") << QString::fromUtf8("ジェ")
		<< QString::fromUtf8("チェ") << QString::fromUtf8("ティ") << QString::fromUtf8("トゥ")
		<< QString::fromUtf8("ディ") << QString::fromUtf8("ドゥ") << QString::fromUtf8("ツァ")
		<< QString::fromUtf8("ツィ") << QString::fromUtf8("ツェ") << QString::fromUtf8("ツォ")
		<< QString::fromUtf8("ファ") << QString::fromUtf8("フィ") << QString::fromUtf8("フェ")
		<< QString::fromUtf8("フォ") << QString::fromUtf8("ヴャ") << QString::fromUtf8("ヴュ")
		<< QString::fromUtf8("ヴョ") << QString::fromUtf8("テュ") << QString::fromUtf8("デュ")
		<< QString::fromUtf8("フュ");

	basicRomaji << QString::fromUtf8("a") << QString::fromUtf8("i") << QString::fromUtf8("u")
		<< QString::fromUtf8("e") << QString::fromUtf8("o") << QString::fromUtf8("ka")
		<< QString::fromUtf8("ki") << QString::fromUtf8("ku") << QString::fromUtf8("ke")
		<< QString::fromUtf8("ko") << QString::fromUtf8("sa") << QString::fromUtf8("shi")
		<< QString::fromUtf8("su") << QString::fromUtf8("se") << QString::fromUtf8("so")
		<< QString::fromUtf8("ta") << QString::fromUtf8("chi") << QString::fromUtf8("tsu")
		<< QString::fromUtf8("te") << QString::fromUtf8("to") << QString::fromUtf8("na")
		<< QString::fromUtf8("ni") << QString::fromUtf8("nu") << QString::fromUtf8("ne")
		<< QString::fromUtf8("no") << QString::fromUtf8("ha") << QString::fromUtf8("hi")
		<< QString::fromUtf8("fu") << QString::fromUtf8("he") << QString::fromUtf8("ho")
		<< QString::fromUtf8("ma") << QString::fromUtf8("mi") << QString::fromUtf8("mu")
		<< QString::fromUtf8("me") << QString::fromUtf8("mo") << QString::fromUtf8("ya")
		<< QString::fromUtf8("yu") << QString::fromUtf8("yo") << QString::fromUtf8("ra")
		<< QString::fromUtf8("ri") << QString::fromUtf8("ru") << QString::fromUtf8("re")
		<< QString::fromUtf8("ro") << QString::fromUtf8("wa") // QString::fromUtf8("n")
		<< QString::fromUtf8("wi") << QString::fromUtf8("we")
		<< QString::fromUtf8("wi") << QString::fromUtf8("we") << QString::fromUtf8("wo")
		<< QString::fromUtf8("ga") << QString::fromUtf8("gi") << QString::fromUtf8("gu")
		<< QString::fromUtf8("ge") << QString::fromUtf8("go") << QString::fromUtf8("za")
		<< QString::fromUtf8("ji") << QString::fromUtf8("zu") << QString::fromUtf8("ze")
		<< QString::fromUtf8("di") << QString::fromUtf8("du")
		<< QString::fromUtf8("zo") << QString::fromUtf8("da") << QString::fromUtf8("ji")
		<< QString::fromUtf8("zu") << QString::fromUtf8("de") << QString::fromUtf8("do")
		<< QString::fromUtf8("ba") << QString::fromUtf8("bi") << QString::fromUtf8("bu")
		<< QString::fromUtf8("be") << QString::fromUtf8("bo") << QString::fromUtf8("pa")
		<< QString::fromUtf8("pi") << QString::fromUtf8("pu") << QString::fromUtf8("pe")
		<< QString::fromUtf8("po");

	youonRomaji << QString::fromUtf8("kya") << QString::fromUtf8("kyu") << QString::fromUtf8("kyo")
		<< QString::fromUtf8("sha") << QString::fromUtf8("shu") << QString::fromUtf8("sho")
		<< QString::fromUtf8("cha") << QString::fromUtf8("chu") << QString::fromUtf8("cho")
		<< QString::fromUtf8("nya") << QString::fromUtf8("nyu") << QString::fromUtf8("nyo")
		<< QString::fromUtf8("hya") << QString::fromUtf8("hyu") << QString::fromUtf8("hyo")
		<< QString::fromUtf8("mya") << QString::fromUtf8("myu") << QString::fromUtf8("myo")
		<< QString::fromUtf8("rya") << QString::fromUtf8("ryu") << QString::fromUtf8("ryo")
		<< QString::fromUtf8("gya") << QString::fromUtf8("gyu") << QString::fromUtf8("gyo")
		<< QString::fromUtf8("ja") << QString::fromUtf8("ju") << QString::fromUtf8("jo")
		<< QString::fromUtf8("ja") << QString::fromUtf8("ju") << QString::fromUtf8("jo")
		<< QString::fromUtf8("bya") << QString::fromUtf8("byu") << QString::fromUtf8("byo")
		<< QString::fromUtf8("pya") << QString::fromUtf8("pyu") << QString::fromUtf8("pyo");

	modernRomaji << QString::fromUtf8("ye") << QString::fromUtf8("wi") << QString::fromUtf8("we")
		<< QString::fromUtf8("wo") << QString::fromUtf8("va") << QString::fromUtf8("vi")
		<< QString::fromUtf8("vu") << QString::fromUtf8("ve") << QString::fromUtf8("vo")
		<< QString::fromUtf8("va") << QString::fromUtf8("vi") << QString::fromUtf8("ve")
		<< QString::fromUtf8("vo") << QString::fromUtf8("she") << QString::fromUtf8("je")
		<< QString::fromUtf8("che") << QString::fromUtf8("ti") << QString::fromUtf8("tu")
		<< QString::fromUtf8("di") << QString::fromUtf8("du") << QString::fromUtf8("tsa")
		<< QString::fromUtf8("tsi") << QString::fromUtf8("tse") << QString::fromUtf8("tso")
		<< QString::fromUtf8("fa") << QString::fromUtf8("fi") << QString::fromUtf8("fe")
		<< QString::fromUtf8("fo") << QString::fromUtf8("vya") << QString::fromUtf8("vyu")
		<< QString::fromUtf8("vyo") << QString::fromUtf8("tyu") << QString::fromUtf8("dyu")
		<< QString::fromUtf8("fyu");

	smallKana << QString::fromUtf8("ぁ") << QString::fromUtf8("ぃ") << QString::fromUtf8("ぅ")
		<< QString::fromUtf8("ぇ") << QString::fromUtf8("ぉ") << QString::fromUtf8("ゃ")
		<< QString::fromUtf8("ゅ") << QString::fromUtf8("ょ") << QString::fromUtf8("っ")
		<< QString::fromUtf8("ァ") << QString::fromUtf8("ィ") << QString::fromUtf8("ゥ")
		<< QString::fromUtf8("ェ") << QString::fromUtf8("ォ") << QString::fromUtf8("ャ")
		<< QString::fromUtf8("ュ") << QString::fromUtf8("ョ") << QString::fromUtf8("ッ");

	smallRomaji << QString::fromUtf8("a") << QString::fromUtf8("i") << QString::fromUtf8("u")
		<< QString::fromUtf8("e") << QString::fromUtf8("o") << QString::fromUtf8("ya")
		<< QString::fromUtf8("yu") << QString::fromUtf8("yo") << QString::fromUtf8("tsu")
		<< QString::fromUtf8("A") << QString::fromUtf8("I") << QString::fromUtf8("U")
		<< QString::fromUtf8("E") << QString::fromUtf8("O") << QString::fromUtf8("YA")
		<< QString::fromUtf8("YU") << QString::fromUtf8("YO") << QString::fromUtf8("TSU");

	specialKana << QString::fromUtf8(" he ") << QString::fromUtf8(" ha ") << QString::fromUtf8(" wo ")
		<< QString::fromUtf8("。") // We removed the middle dot here - QString::fromUtf8("・")
		<< QString::fromUtf8("、") << QString::fromUtf8("？") << QString::fromUtf8("〜")
		<< QString::fromUtf8("！") << QString::fromUtf8("（") << QString::fromUtf8("）")
		<< QString::fromUtf8("＊") << QString::fromUtf8("「") << QString::fromUtf8("」")
		<< QString::fromUtf8("★") << QString::fromUtf8("☆")  // We removed the space here - QString::fromUtf8("　")
		<< QString::fromUtf8("…") << QString::fromUtf8("『") << QString::fromUtf8("』")
		<< QString::fromUtf8("【") << QString::fromUtf8("】") << QString::fromUtf8("《")
		<< QString::fromUtf8("》");

	specialRomaji << QString::fromUtf8(" e ") << QString::fromUtf8(" wa ") << QString::fromUtf8(" o ")
		<< QString::fromUtf8(".") // We removed the space here - QString::fromUtf8(" ")
		<< QString::fromUtf8(",") << QString::fromUtf8("?") << QString::fromUtf8("~")
		<< QString::fromUtf8("!") << QString::fromUtf8("(") << QString::fromUtf8(")")
		<< QString::fromUtf8("*") << QString::fromUtf8("\"") << QString::fromUtf8("\"")
		<< QString::fromUtf8("*") << QString::fromUtf8("+") // We removed the space here - QString::fromUtf8(" ")
		<< QString::fromUtf8("...") << QString::fromUtf8("[") << QString::fromUtf8("]")
		<< QString::fromUtf8("[") << QString::fromUtf8("]") << QString::fromUtf8("<")
		<< QString::fromUtf8(">");

	tablesLoaded = true;
};

QString romajiToKana(const QString& string, bool special)
{
	setupConversionTables();
	QString text = string;

	// Replace "ō" with "ou"
	text.replace( QString::fromUtf8("ō"), "ou" );

	if(special)
	{
		// Post stuff (things like brackets, punctuation, and small a/i/u/e/o)
		for(int i = 0; i < specialKana.count(); i++)
			text.replace(specialRomaji.at(i), specialKana.at(i));

		// Small kana, a, i, u, e, o
		for(int i = 0; i < smallKana.count(); i++)
		{
			if(i >= 9)
				text.replace("X" + smallRomaji.at(i), smallKana.at(i));
			else
				text.replace("x" + smallRomaji.at(i), smallKana.at(i));
		}
	}

	// Handle katakana long marks (ー)
	text.replace( "AA",  QString::fromUtf8("Aー") );
	text.replace( "II",  QString::fromUtf8("Iー") );
	text.replace( "UU",  QString::fromUtf8("Uー") );
	text.replace( "EE",  QString::fromUtf8("Eー") );
	text.replace( "OO",  QString::fromUtf8("Oー") );
	text.replace( "OU",  QString::fromUtf8("Oー") );

	// Special cases (romaji characters with alternate spellings or > 2 characters)
	text.replace( "si",  QString::fromUtf8("し") );
	text.replace( "ti",  QString::fromUtf8("ち") );
	text.replace( "tu",  QString::fromUtf8("つ") );
	text.replace( "xtu", QString::fromUtf8("っ") );
	text.replace( "hu",  QString::fromUtf8("ふ") );
	text.replace( "dzu",  QString::fromUtf8("づ") );
	text.replace( "tya",  QString::fromUtf8("ちゃ") );
	text.replace( "SI",  QString::fromUtf8("シ") );
	text.replace( "TI",  QString::fromUtf8("チ") );
	text.replace( "TU",  QString::fromUtf8("ツ") );
	text.replace( "XTU", QString::fromUtf8("ッ") );
	text.replace( "HU",  QString::fromUtf8("フ") );
	text.replace( "DZU",  QString::fromUtf8("ヅ") );
	text.replace( "TYA",  QString::fromUtf8("チャ") );

	text.replace( "shi", QString::fromUtf8("し") );
	text.replace( "chi", QString::fromUtf8("ち") );
	text.replace( "tsu", QString::fromUtf8("つ") );
	text.replace( "SHI", QString::fromUtf8("シ") );
	text.replace( "CHI", QString::fromUtf8("チ") );
	text.replace( "TSU", QString::fromUtf8("ツ") );

	// Modern Katakana
    for(int i = modernKatakana.count() - 1; i >= 0; i--)
        text.replace(modernRomaji.at(i).toUpper(), modernKatakana.at(i));

	// Youon Hiragana
    for(int i = youonHiragana.count() - 1; i >= 0; i--)
        text.replace(youonRomaji.at(i), youonHiragana.at(i));

	// Youon Katakana
    for(int i = youonKatakana.count() - 1; i >= 0; i--)
        text.replace(youonRomaji.at(i).toUpper(), youonKatakana.at(i));

	// Basic Hiragana
    for(int i = basicHiragana.count() - 1; i >= 0; i--)
        text.replace(basicRomaji.at(i), basicHiragana.at(i));

    // Basic Katakana
    for(int i = basicKatakana.count() - 1; i >= 0; i--)
        text.replace(basicRomaji.at(i).toUpper(), basicKatakana.at(i));

	// Do the special stuff with n
	text.replace( "nn", QString::fromUtf8("ん") );
	text.replace( "n'", QString::fromUtf8("ん") );
	text.replace( "n",  QString::fromUtf8("ん") );
	text.replace( "NN", QString::fromUtf8("ン") );
	text.replace( "N'", QString::fromUtf8("ン") );
	text.replace( "N",  QString::fromUtf8("ン") );

	/// @TODO These might need some work to only change the right stuff,
	/// but I don't see much of a problem as you shouldn't be entering in
	/// text that isn't valid romaji, and therfore you shouldn't be expecting
	/// some weird output with っ and ッ in odd places. This could interfere
	/// with katakana long marks.
	//text.replace( QRegExp("[a-z]([\\x3040-\\x309F])"), QString::fromUtf8("っ\\1") );
	//text.replace( QRegExp("[A-Z]([\\x30A0-\\x30FF])"), QString::fromUtf8("ッ\\1" ) );
	text.replace( QRegExp("[kstcnhmyrwgzdbp]([\\x3040-\\x309F])"), QString::fromUtf8("っ\\1") );
	text.replace( QRegExp("[KSTCNHMYRWGZDBP]([\\x30A0-\\x30FF])"), QString::fromUtf8("ッ\\1" ) );
	//text.replace( QRegExp( QString::fromUtf8("[a-z]([ぁ–ゞ])"), QString::fromUtf8("っ\\1") );
	//text.replace( QRegExp( QString::fromUtf8("[A-Z]([ァ–ヾ])"), QString::fromUtf8("ッ\\1" );

	return text;
}

QString kanaToRomaji(const QString& string, bool special)
{
	setupConversionTables();
	QString text = string;

    // Modern Katakana
    for(int i = 0; i < modernKatakana.count(); i++)
        text.replace(modernKatakana.at(i), modernRomaji.at(i).toUpper());

    // Youon Hiragana
    for(int i = 0; i < youonHiragana.count(); i++)
        text.replace(youonHiragana.at(i), youonRomaji.at(i));

    // Youon Katakana
    for(int i = 0; i < youonKatakana.count(); i++)
        text.replace(youonKatakana.at(i), youonRomaji.at(i).toUpper());

    // Basic Hiragana
    for(int i = 0; i < basicHiragana.count(); i++)
        text.replace(basicHiragana.at(i), basicRomaji.at(i));

    // Basic Katakana
    for(int i = 0; i < basicKatakana.count(); i++)
        text.replace(basicKatakana.at(i), basicRomaji.at(i).toUpper());

	// These are a special case because when we go romaji->kana they cause problems in the main list
	text.replace( QRegExp( QString::fromUtf8("ん") ), "n" );
	text.replace( QRegExp( QString::fromUtf8("ン") ), "N" );

    // http://en.wikipedia.org/wiki/Talk:Hepburn_romanization
    // "Double (or "geminate") consonants are marked by doubling the consonant
    // following the sokuon, っ, except for sh→ssh, ch→tch, ts→tts.."

	// Hiragana
	text.replace( QRegExp( QString::fromUtf8("っsh") ), "ssh" );
	text.replace( QRegExp( QString::fromUtf8("っch") ), "tch" );
	text.replace( QRegExp( QString::fromUtf8("っts") ), "tts" );
    text.replace( QRegExp( QString::fromUtf8("っ(.{1})") ), "\\1\\1" );
	// Katakana
	text.replace( QRegExp( QString::fromUtf8("ッSH") ), "SSH" );
	text.replace( QRegExp( QString::fromUtf8("ッCH") ), "TCH" );
	text.replace( QRegExp( QString::fromUtf8("ッTS") ), "TTS" );
    text.replace( QRegExp( QString::fromUtf8("ッ(.{1})") ), "\\1\\1" );

	// Katakana long marks
    text.replace( QRegExp("/(.{1})ー/"), "\\1\\1" );

	// Replace "oo" and "ou" with "ō"
	// text.replace( "oo", QString::fromUtf8("ō") );
	// text.replace( "ou", QString::fromUtf8("ō") );

	if(!special)
		return text;

	// Post stuff (can't be before modern katakana because they use small a/i/u/e/o)
    for(int i = 0; i < specialKana.count(); i++)
		text.replace(specialKana.at(i), specialRomaji.at(i));

	// Small kana, a, i, u, e, o
	for(int i = 0; i < smallKana.count(); i++)
		text.replace(smallKana.at(i), smallRomaji.at(i));

	// We didn't put the space conversion into the mix because it would cause problems with romaji->kana conversion
	text.replace( QString::fromUtf8("　"), " " );

    return text;
}

QString katakanaToHiragana(const QString& string)
{
	setupConversionTables();
	QString text = string;

    // Youon Katakana
    for(int i = 0; i < youonKatakana.count(); i++)
        text.replace(youonKatakana.at(i), youonHiragana.at(i));

    // Basic Katakana
    for(int i = 0; i < basicKatakana.count(); i++)
        text.replace(basicKatakana.at(i), basicHiragana.at(i));

	// Small Katakana
	int smallKanaCount = smallKana.count() / 2;
	for(int i = 0; i < smallKanaCount; i++)
		text.replace(smallKana.at(i + smallKanaCount), smallKana.at(i));

	text.replace( QRegExp( QString::fromUtf8("ン") ), QString::fromUtf8("ん") );

	return text;
};

QString hiraganaToKatakana(const QString& string)
{
	setupConversionTables();
	QString text = string;

    // Youon Hiragana
    for(int i = 0; i < youonHiragana.count(); i++)
        text.replace(youonHiragana.at(i), youonKatakana.at(i));

    // Basic Hiragana
    for(int i = 0; i < basicHiragana.count(); i++)
        text.replace(basicHiragana.at(i), basicKatakana.at(i));

	// Small Hiragana
	int smallKanaCount = smallKana.count() / 2;
	for(int i = 0; i < smallKanaCount; i++)
		text.replace(smallKana.at(i), smallKana.at(i + smallKanaCount));

	text.replace( QRegExp( QString::fromUtf8("ん") ), QString::fromUtf8("ン") );

	return text;
};

bool containsRuby(const QString& text)
{
	QRegExp rubyRx("<ruby>(.*)</ruby>");
	rubyRx.setMinimal(true);
	if(rubyRx.indexIn(text) != -1)
		return true;

	QRegExp wikiRubyMatcher("\\{\\{ruby\\|([^\\}\\|]+)\\|([^\\}\\|]+)\\}\\}");
	if(wikiRubyMatcher.indexIn(text) != -1)
		return true;

	QRegExp shorthandRubyMatcher("\\{([^\\}\\|]+)\\|([^\\}\\|]+)\\}");
	if(shorthandRubyMatcher.indexIn(text) != -1)
		return true;

	return false;
};

QString parseRuby(const QString& string, RubyFormat format)
{
	QString final = string;

	// Replace our wiki template with normal ruby
	if(format != Wiki)
	{
		QRegExp wikiRubyMatcher("\\{\\{ruby\\|([^\\}\\|]+)\\|([^\\}\\|]+)\\}\\}");
		wikiRubyMatcher.setMinimal(true);

		if(format == NoParentheses)
			final.replace(wikiRubyMatcher, "<ruby><rb>\\1</rb><rt>\\2</rt></ruby>");
		else
			final.replace(wikiRubyMatcher, "<ruby><rb>\\1</rb><rp>(</rp><rt>\\2</rt><rp>)</rp></ruby>");
	}

	// Replace our short hand with normal ruby
	if(format != ShortHand)
	{
		QRegExp shorthandRubyMatcher("\\{([^\\}\\|]+)\\|([^\\}\\|]+)\\}");
		shorthandRubyMatcher.setMinimal(true);

		if(format == NoParentheses)
			final.replace(shorthandRubyMatcher, "<ruby><rb>\\1</rb><rt>\\2</rt></ruby>");
		else
			final.replace(shorthandRubyMatcher, "<ruby><rb>\\1</rb><rp>(</rp><rt>\\2</rt><rp>)</rp></ruby>");
	}

	QRegExp rubyRx("<ruby>(.*)</ruby>");
	rubyRx.setMinimal(true);
	QRegExp bottomRx("<rb>(.*)</rb>");
	bottomRx.setMinimal(true);
	QRegExp topRx("<rt>(.*)</rt>");
	topRx.setMinimal(true);
	QRegExp parenthesesRx("<rp>(.*)</rp>");
	parenthesesRx.setMinimal(true);

	int pos = 0;
	QString insideText;
	QString replacement;
	while( (pos = rubyRx.indexIn(final, pos)) != -1 )
	{
		insideText = rubyRx.cap(1);
		replacement.clear();

		switch(format)
		{
			case Normal:
				replacement += "<ruby>" + insideText + "</ruby>";
				break;
			case Flat:
				insideText.replace(bottomRx, "\\1");
				insideText.replace(topRx, "\\1");
				insideText.replace(parenthesesRx, "\\1");
				replacement += insideText;
				break;
			case Bottom:
				insideText.replace(bottomRx, "\\1");
				insideText.replace(topRx, QString());
				insideText.replace(parenthesesRx, QString());
				replacement += insideText;
				break;
			case Top:
				insideText.replace(bottomRx, QString());
				insideText.replace(topRx, "\\1");
				insideText.replace(parenthesesRx, QString());
				replacement += insideText;
				break;
			case ShortHand:
				insideText.replace(bottomRx, "\\1|");
				insideText.replace(topRx, "\\1");
				insideText.replace(parenthesesRx, QString());
				replacement += "{" + insideText + "}";
				break;
			case Wiki:
				insideText.replace(bottomRx, "\\1|");
				insideText.replace(topRx, "\\1");
				insideText.replace(parenthesesRx, QString());
				replacement += "{{ruby|" + insideText + "}}";
				break;
			case NoParentheses:
				insideText.replace(parenthesesRx, QString());
				replacement += "<ruby>" + insideText + "</ruby>";
				break;
		}

		final.replace(pos, rubyRx.matchedLength(), replacement);
		pos += replacement.length();
	}

	return final;
};

QString reduceRuby(const QString& bottom, const QString& top)
{
	if( !top.count() )
		return bottom;

	int topStart = 0;
	int firstOccurrence = 0;
	int bottomStart = 0, bottomSpan = 0;
	QString currentTop, currentBottom;
	QString final;

	while( bottomStart < bottom.count() )
	{
		while( bottom.at(bottomStart) == top.at(topStart) )
		{
			currentBottom += bottom.at(bottomStart);
			bottomStart++; topStart++;

			if( bottomStart >= bottom.count() )
				break;
		}
		final += currentBottom;
		currentBottom.clear();

		if( bottomStart >= bottom.count() )
			break;

		while( ( firstOccurrence = top.mid(topStart).indexOf( bottom.mid(bottomStart + bottomSpan, 1) ) ) == -1)
		{
			bottomSpan++;

			if( bottomStart + bottomSpan >= bottom.count() )
			{
				firstOccurrence = top.mid(topStart).count();
				break;
			}
		}

		// if(firstOccurrence == -1)
			// return QObject::tr("Parsing Error");

		currentBottom = bottom.mid(bottomStart, bottomSpan);
		bottomStart += bottomSpan;
		bottomSpan = 0;

		currentTop = top.mid(topStart, firstOccurrence);
		topStart += firstOccurrence;

		final += "<ruby><rb>" + currentBottom + "</rb><rp>(</rp><rt>" + currentTop + "</rt><rp>)</rp></ruby>";
		currentBottom.clear(); currentTop.clear();
	}

	return final;
};
