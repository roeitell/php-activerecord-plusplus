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