const char * activerecord_uncountable[9] = {"fish", "sheep", "deer", "series", "species", "money", "rice", "information", "equipment" };
const char * activerecord_irregulars[8] = { "move", "foot", "goose", "sex", "child", "man", "tooth", "person" } ;
const char * activerecord_irregular_plurals[8] = { "moves", "feet", "geese", "sexes", "children", "men", "teeth", "people" } ;
const char * activerecord_plural_matches[19] = { 
	"/(quiz)$/i", "/^(ox)$/i", "/([m|l])ouse$/i", "/(matr|vert|ind)ix|ex$/i", "/(x|ch|ss|sh)$/i", "/([^aeiouy]|qu)y$/i", "/(hive)$/i", 
	"/(?:([^f])fe|([lr])f)$/i", "/(shea|lea|loa|thie)f$/i", "/sis$/i", "/([ti])um$/i", "/(tomat|potat|ech|her|vet)o$/i", "/(bu)s$/i", "/(alias)$/i", 
	"/(octop)us$/i", "/(ax|test)is$/i", "/(us)$/i", "/s$/i", "/$/"
};
const char * activerecord_plural_replacements[19] = {
	"$1zes", "$1en", "$1ice", "$1ices", "$1es", "$1ies", "$1s", "$1$2ves", "$1ves", "ses", "$1a", "$1oes", "$1ses", "$1es", "$1i", "$1es", 
	"$1es", "s", "s"
};
const char * activerecord_singular_matches[28] = {
	"/(quiz)zes$/i", "/(matr)ices$/i", "/(vert|ind)ices$/i", "/^(ox)en$/i", "/(alias)es$/i", "/(octop|vir)i$/i", "/(cris|ax|test)es$/i", "/(shoe)s$/i", 
	"/(o)es$/i", "/(bus)es$/i", "/([m|l])ice$/i", "/(x|ch|ss|sh)es$/i", "/(m)ovies$/i", "/(s)eries$/i", "/([^aeiouy]|qu)ies$/i", "/([lr])ves$/i", 
	"/(tive)s$/i", "/(hive)s$/i", "/(li|wi|kni)ves$/i", "/(shea|loa|lea|thie)ves$/i", "/(^analy)ses$/i", 
	"/((a)naly|(b)a|(d)iagno|(p)arenthe|(p)rogno|(s)ynop|(t)he)ses$/i", "/([ti])a$/i", "/(n)ews$/i", "/(h|bl)ouses$/i", "/(corpse)s$/i", "/(us)es$/i", "/s$/i"
};
const char * activerecord_singular_replacements[28] = {
	"$1", "$1ix", "$1ex", "$1", "$1", "$1us", "$1is", "$1", "$1", "$1", "$1ouse", "$1", "$1ovie", "$1eries", "$1y", "$1f", "$1", "$1", "$1fe", 
	"$1f", "$1sis", "$1$2sis", "$1um", "$1ews", "$1ouse", "$1", "$1", ""
};

/*


activerecord_singular_matches[0] = "/(quiz)zes$/i";             		activerecord_singular_replacements[0] =  "$1";
activerecord_singular_matches[1] = "/(matr)ices$/i";            		activerecord_singular_replacements[1] =  "$1ix";
activerecord_singular_matches[2] = "/(vert|ind)ices$/i";        		activerecord_singular_replacements[2] =  "$1ex";
activerecord_singular_matches[3] = "/^(ox)en$/i";               		activerecord_singular_replacements[3] =  "$1";
activerecord_singular_matches[4] = "/(alias)es$/i";            		activerecord_singular_replacements[4] =  "$1";
activerecord_singular_matches[5] = "/(octop|vir)i$/i";          		activerecord_singular_replacements[5] =  "$1us";
activerecord_singular_matches[6] = "/(cris|ax|test)es$/i";      		activerecord_singular_replacements[6] =  "$1is";
activerecord_singular_matches[7] = "/(shoe)s$/i";               		activerecord_singular_replacements[7] =  "$1";
activerecord_singular_matches[8] = "/(o)es$/i";                 		activerecord_singular_replacements[8] =  "$1";
activerecord_singular_matches[9] = "/(bus)es$/i";               		activerecord_singular_replacements[9] =  "$1";
activerecord_singular_matches[10] = "/([m|l])ice$/i";            	activerecord_singular_replacements[10] =  "$1ouse";
activerecord_singular_matches[11] = "/(x|ch|ss|sh)es$/i";        	activerecord_singular_replacements[11] =  "$1";
activerecord_singular_matches[12] = "/(m)ovies$/i";              	activerecord_singular_replacements[12] =  "$1ovie";
activerecord_singular_matches[13] = "/(s)eries$/i";              	activerecord_singular_replacements[13] =  "$1eries";
activerecord_singular_matches[14] = "/([^aeiouy]|qu)ies$/i";     	activerecord_singular_replacements[14] =  "$1y";
activerecord_singular_matches[15] = "/([lr])ves$/i";             	activerecord_singular_replacements[15] =  "$1f";
activerecord_singular_matches[16] = "/(tive)s$/i";               	activerecord_singular_replacements[16] =  "$1";
activerecord_singular_matches[17] = "/(hive)s$/i";               	activerecord_singular_replacements[17] =  "$1";
activerecord_singular_matches[18] = "/(li|wi|kni)ves$/i";        	activerecord_singular_replacements[18] =  "$1fe";
activerecord_singular_matches[19] = "/(shea|loa|lea|thie)ves$/i";	activerecord_singular_replacements[19] =  "$1f";
activerecord_singular_matches[20] = "/(^analy)ses$/i";           	activerecord_singular_replacements[20] =  "$1sis";
activerecord_singular_matches[21] = "/((a)naly|(b)a|(d)iagno|(p)arenthe|(p)rogno|(s)ynop|(t)he)ses$/i";  
														activerecord_singular_replacements[21] =  "$1$2sis";
activerecord_singular_matches[22] = "/([ti])a$/i";               	activerecord_singular_replacements[22] =  "$1um";
activerecord_singular_matches[23] = "/(n)ews$/i";                	activerecord_singular_replacements[23] =  "$1ews";
activerecord_singular_matches[24] = "/(h|bl)ouses$/i";           	activerecord_singular_replacements[24] =  "$1ouse";
activerecord_singular_matches[25] = "/(corpse)s$/i";             	activerecord_singular_replacements[25] =  "$1";
activerecord_singular_matches[26] = "/(us)es$/i";                	activerecord_singular_replacements[26] =  "$1";
activerecord_singular_matches[27] = "/s$/i";                     	activerecord_singular_replacements[27] =  "";


activerecord_uncountable[0] = "fish";
activerecord_uncountable[1] = "sheep";
activerecord_uncountable[2] = "deer";
activerecord_uncountable[3] = "series";
activerecord_uncountable[4] = "species";
activerecord_uncountable[5] = "money";
activerecord_uncountable[6] = "rice";
activerecord_uncountable[7] = "information";
activerecord_uncountable[8] = "equipment";

activerecord_irregulars[0] = "move";		activerecord_irregular_plurals[0] = "moves";
activerecord_irregulars[1] = "foot";		activerecord_irregular_plurals[1] = "feet";
activerecord_irregulars[2] = "goose";	activerecord_irregular_plurals[2] = "geese";
activerecord_irregulars[3] = "sex";		activerecord_irregular_plurals[3] = "sexes";
activerecord_irregulars[4] = "child";	activerecord_irregular_plurals[4] = "children";
activerecord_irregulars[5] = "man";		activerecord_irregular_plurals[5] = "men";
activerecord_irregulars[6] = "tooth";	activerecord_irregular_plurals[6] = "teeth";
activerecord_irregulars[7] = "person";	activerecord_irregular_plurals[7] = "people";

activerecord_plural_matches[0] = "/(quiz)$/i"; 						activerecord_plural_replacements[0] = "$1zes";
activerecord_plural_matches[1] = "/^(ox)$/i";						activerecord_plural_replacements[1] = "$1en";
activerecord_plural_matches[2] = "/([m|l])ouse$/i";					activerecord_plural_replacements[2] = "$1ice";
activerecord_plural_matches[3] = "/(matr|vert|ind)ix|ex$/i";			activerecord_plural_replacements[3] = "$1ices";
activerecord_plural_matches[4] = "/(x|ch|ss|sh)$/i";					activerecord_plural_replacements[4] = "$1es";
activerecord_plural_matches[5] = "/([^aeiouy]|qu)y$/i";				activerecord_plural_replacements[5] = "$1ies";
activerecord_plural_matches[6] = "/(hive)$/i";						activerecord_plural_replacements[6] = "$1s";
activerecord_plural_matches[7] = "/(?:([^f])fe|([lr])f)$/i";			activerecord_plural_replacements[7] = "$1$2ves";
activerecord_plural_matches[8] = "/(shea|lea|loa|thie)f$/i";			activerecord_plural_replacements[8] = "$1ves";
activerecord_plural_matches[9] = "/sis$/i";							activerecord_plural_replacements[9] = "ses";
activerecord_plural_matches[10] = "/([ti])um$/i";					activerecord_plural_replacements[10] = "$1a";
activerecord_plural_matches[11] = "/(tomat|potat|ech|her|vet)o$/i";	activerecord_plural_replacements[11] = "$1oes";
activerecord_plural_matches[12] = "/(bu)s$/i";						activerecord_plural_replacements[12] = "$1ses";
activerecord_plural_matches[13] = "/(alias)$/i";						activerecord_plural_replacements[13] = "$1es";
activerecord_plural_matches[14] = "/(octop)us$/i";					activerecord_plural_replacements[14] = "$1i";
activerecord_plural_matches[15] = "/(ax|test)is$/i";					activerecord_plural_replacements[15] = "$1es";
activerecord_plural_matches[16] = "/(us)$/i";						activerecord_plural_replacements[16] = "$1es";
activerecord_plural_matches[17] = "/s$/i";							activerecord_plural_replacements[17] = "s";
activerecord_plural_matches[18] = "/$/";								activerecord_plural_replacements[18] = "s";

*/