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

// Public domain translations (KJV,ASV,YLT,DARBY,DR,ELB,LUTH1545,LSG,RVA,CEI,VULGATE)
static bool g_bAvailTrans [] = {
	false, // Todays New International Version
		false, // New International Version
		false, // New American Standard Bible
		false, // The Message
		false, // Amplified Bible
		false, // New Living Translation
		true, // King James Version (Public Domain)
		true, // King James Version Apocrypha (Public Domain)
		false, // English Standard Version
		false, // Contemporary English Version
		false, // New King James Version
		false, // 21st Century King James Version
		false, // American Standard Version (Public Domain)
		false, // Youngs Literal Translation (Public Domain)
		false, // Darby Translation (Public Domain)
		false, // New Life Version
		false, // Holman Christian Standard Bible
		false, // New Internation Readers Version
		false, // New International Version UK
		false, // Wycliffe New Testament
		false, // Worldwide English New Testament
		false, // Douay-Rheims 1899 American edition (Public Domain)
		false, // Elberfelder (Public Domain)
		false, // Hoffnung fur Alle
		false, // Luther Bibel 1545 (Public Domain)
		false, // La Bible du Semeur
		false, // Louis Segond (Public Domain)
		false, // Biblia en Lenguaje Sencillo
		false, // Castillian
		false, // Dios Habla Hoy
		false, // La Biblia De Las Americas
		false, // Nueva Version Internacional
		false, // Reina-Valera 1960
		false, // Reina-Valera 1995
		false, // Reina-Valera Antigua (Public Domain)
		false, // Conferenza Episcopale Italiana (Public Domain)
		false, // La Nuova Diodati (Italian No copyright information)
		false, // La Parola è Vita (Italian IBS)
		false // Biblia Sacra Vulgata (Public Domain)
};
