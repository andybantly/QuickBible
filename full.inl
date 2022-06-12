static const char * g_pDesc [] = {
	"Todays New International Version","New International Version","New American Standard Bible",
		"The Message","Amplified Bible","New Living Translation","King James Version","King James Version Apocrypha",
		"English Standard Version","Contemporary English Version","New King James Version",
		"21st Century King James Version","American Standard Version","Youngs Literal Translation",
		"Darby Translation","New Life Version","Holman Christian Standard Bible",
		"New Internation Readers Version","New International Version UK","Wycliffe New Testament",
		"Worldwide English New Testament","Douay-Rheims", // End of English
		"Elberfelder", "Hoffnung fur Alle",	"Luther Bibel 1545", // End of German
		"La Bible du Semeur", "Louis Segond", // End of French
		"Biblia en Lenguaje Sencillo", "Castillian", "Dios Habla Hoy", "La Biblia De Las Americas",
		"Nueva Version Internacional", "Reina-Valera 1960", "Reina-Valera 1995",
		"Reina-Valera Antigua", // End of Spanish
		"Conferenza Episcopale Italiana",
		"La Nuova Diodati",
		"La Parola è Vita", // End of Italian
		"Biblia Sacra Vulgata" // Latin
};

static const char * g_pTrans[] = {
	"TNIV","NIV","NASB","MSG","AMP","NLT","KJV","APOC","ESV","CEV","NKJV","KJ21","ASV","YLT","DARBY", // English
		"NLV","HCSB","NIRV","NIVUK","WYC","WE","DRA", // English
		"ELB","HOF","LUTH1545", // German
		"BDS","LSG", // French
		"BLS","CST","DHH","LBLA","NVI","RVR1960","RVR1995","RVA", // Spanish
		"CEI","LND","LM", // Italian
		"VULGATE" // Latin
};

// Public domain translations (KJV,ASV,YLT,DARBY,DR,ELB,LUTH1545,LSG,RVA)
static bool g_bAvailTrans [] = {
	true, // Todays New International Version
		true, // New International Version
		true, // New American Standard Bible
		true, // The Message
		true, // Amplified Bible
		true, // New Living Translation
		true, // King James Version (Public Domain)
		true, // King James Version Apocrypha (Public Domain)
		true, // English Standard Version
		true, // Contemporary English Version
		true, // New King James Version
		true, // 21st Century King James Version
		true, // American Standard Version (Public Domain)
		true, // Youngs Literal Translation (Public Domain)
		true, // Darby Translation (Public Domain)
		true, // New Life Version
		true, // Holman Christian Standard Bible
		true, // New Internation Readers Version
		true, // New International Version UK
		true, // Wycliffe New Testament
		true, // Worldwide English New Testament
		true, // Douay-Rheims 1899 American edition (Public Domain)
		true, // Elberfelder (Public Domain)
		true, // Hoffnung fur Alle
		true, // Luther Bibel 1545 (Public Domain)
		true, // La Bible du Semeur
		true, // Louis Segond (Public Domain)
		true, // Biblia en Lenguaje Sencillo
		true, // Castillian
		true, // Dios Habla Hoy
		true, // La Biblia De Las Americas
		true, // Nueva Version Internacional
		true, // Reina-Valera 1960
		true, // Reina-Valera 1995
		true, // Reina-Valera Antigua (Public Domain)
		true, // Conferenza Episcopale Italiana (Public Domain)
		true, // La Nuova Diodati (Italian No copyright information)
		true, // La Parola è Vita (Italian IBS)
		true // Biblia Sacra Vulgata (Public Domain)
};
