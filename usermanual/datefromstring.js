// Standard Date object, which offers useful formatting in its toString method, was
// extended with several private members (good encapsulation!) needed to parse the many
// available date/time string formats offered by the new public initialize () method.
// Note, the return value should be used to determine if the date was parsed correctly!

var NOT_DEFINED = -1;
 
var TOKEN_AM = "am";
var TOKEN_PM = "pm";
var TOKEN_SPACE_AM = " am";
var TOKEN_SPACE_PM = " pm";

Date.prototype.initialize = function (isEnglish, dateTimeString) {
   isEnglish = isEnglish;
   dateTimeString = dateTimeString.toLowerCase ();
   that = this; // common workaround for ECMAScript Language spec bug
   valid = false;
   iDTS = 0; // used by parse_* functions to index dateTimeString []
   iToken = 0; // used by initialize_* functions to index tokens []
   tokens = new Array (); // parsed tokens
   signature = ""; // example is "nn/nn/nnnn" for "MM/DD/YYYY"

   if (string_into_tokens (that)) {
      if (initialize_from_tokens (that)) {
         valid = true;
      }
   }
   return valid;

   function initialize_date (signature_date) {
      var m = NOT_DEFINED, d = NOT_DEFINED, y = NOT_DEFINED;
      if (("nn-nn-nnnn" == signature_date) ||
          ("nn/nn/nnnn" == signature_date) ||
          ("nn-nn-nn" == signature_date) ||
          ("nn/nn/nn" == signature_date)) {
         m = 0;
         d = 2;
         y = 4;
         iToken += 5;
      } else if (("nn-nn" == signature_date) ||
                 ("nn/nn" == signature_date)) {
         m = 0;
         d = 2;
         iToken += 3;
      } else if (("nnnn-nn-nn" == signature_date) ||
                 ("nnnn/nn/nn" == signature_date)) {
         y = 0;
         m = 2;
         d = 4;
         iToken += 5;
      } else if (("nnnn-nn" == signature_date) ||
                 ("nnnn/nn" == signature_date)) {
         y = 0;
         m = 2;
         iToken += 3;
      } else if ("m nn, nnnn" == signature_date) {
         m = 0;
         d = 2;
         y = 5;
         iToken += 6;
      } else if (("m nn,nnnn" == signature_date) ||
                 ("m nn nnnn" == signature_date)) {
         m = 0;
         d = 2;
         y = 4;
         iToken += 5;
      } else if ("m nn" == signature_date) {
         m = 0;
         d = 2;
         iToken += 3;
      } else if ("nn" == signature_date) {
         d = 0;
         iToken += 1;
      }

      var success = true;
      if ((y >= 0) || (m >= 0) || (d >= 0)) {

         if (!isEnglish && (m >= 0) && (d >= 0)) {
            var tmp = m;
            m = d;
            d = tmp;
         }

         var yr = that.getYear () + 1900; // offset for setYear () is 0, for getYear () it is 1900
         var mo = that.getMonth ();
         var dy = that.getDate ();

         // unspecified longer time units are set to current year/month, but
         // unspecified shorter time units are set to zero
         if (y >= 0) { yr = parseInt (tokens [y]    ); mo = 0; dy = 0; }
         if (m >= 0) { mo = parseInt (tokens [m] - 1); dy = 0; }
         if (d >= 0) { dy = parseInt (tokens [d]    ); }
         that.setYear   (yr);
         that.setMonth  (mo);
         that.setDate   (dy);
         that.setHours  (0);
         that.setMinutes(0);
         that.setSeconds(0);
      }
   };

   function initialize_from_tokens () {
      // parse out date and time signatures. If there are both date and time, they will be 
      // separated by last space before end, or " am", or " pm". Note that time must always
      // have a colon to eliminate ambiguities
      var start = signature.indexOf (TOKEN_SPACE_AM);
      if (start < 0) { start = signature.indexOf (TOKEN_SPACE_PM); }
      if (start < 0) { start = signature.length - 1; }
      var colon = signature.lastIndexOf (":", start);
      var space = -1;
      if (colon >= 0) {
         space = signature.lastIndexOf (" ", colon);
      }
      if (space >= 0) {
         signature_date = signature.substr (0, space);
         signature_time = signature.substr (space + 1);
      } else {
         if (signature.indexOf (":") < 0) { // date only
            signature_date = signature;
            signature_time = "";
         } else { // time only
            signature_date = "";
            signature_time = signature;
         }
      }
      
      initialize_date (signature_date);
      if ((signature_date.length > 0) && (signature_time.length > 0)) {
         ++iToken; // account for space delimiter
      }
      initialize_time (signature_time);

      var success = (iToken == tokens.length);
      
      dc_log ("date signature = <" + signature_date + 
              ">, time signature = <" + signature_time + 
              ">, successful parsing = " + success);

      return success;
   };

   function initialize_time (signature_time) {
      var h = NOT_DEFINED, m = NOT_DEFINED, s = NOT_DEFINED;
      if ("nn:nn:nn ap" == signature_time) {
         h = iToken;
         m = iToken + 2;
         s = iToken + 4;
         if (tokens [iToken + 6] == TOKEN_PM) { s += 12; }
         iToken += 7;
      } else if ("nn:nn:nnap" == signature_time) {
         h = iToken;
         m = iToken + 2;
         s = iToken + 4;
         if (tokens [iToken + 5] == TOKEN_PM) { s += 12; }
         iToken += 6;
      } else if ("nn:nn:nn" == signature_time) {
         h = iToken;
         m = iToken + 2;
         s = iToken + 4;
         iToken += 5;
      } else if ("nn:nn" == signature_time) {
         h = iToken;
         m = iToken + 2;
         iToken += 3;
      } else if ("nn:" == signature_time) {
         h = iToken;
         iToken += 2;
      }
      if ((h >= 0) || (m >= 0) || (s >= 0)) {

         var hr = that.getHours ();
         var mn = that.getMinutes ();
         var sc = that.getSeconds ();

         // unspecified longer time units are set to current hours/minutes, but
         // unspecified shorter time units are set to zero
         if (h >= 0) { hr = parseInt (tokens [h]); mn = 0; sc = 0; }
         if (m >= 0) { mn = parseInt (tokens [m]); sc = 0; }
         if (s >= 0) { sc = parseInt (tokens [s]); }
         
         that.setHours  (hr);
         that.setMinutes (mn);
         that.setSeconds (sc);
      }
   };

   function is_digit (c) {
      var regexp = /^[0-9]$/;
      var result = c.match (regexp);
      if (null == result) result = false;
      return result;                
   };
   
   function parse_delimiter () {
      var c = dateTimeString.substr (iDTS, 1);
      if (("/" == c) || ("-" == c) || (" " == c) || ("," == c) || (":" == c)) {
         tokens.push (c);
         signature += c;
         ++iDTS;
         return true;
      }
      return false;
   };

   function parse_integer () {
      var foundInteger = false;
      var sum = 0;
      while ((iDTS < dateTimeString.length) && 
             (is_digit (dateTimeString.substr (iDTS, 1)))) {
         sum = 10 * sum + parseInt (dateTimeString.substr (iDTS, 1));
         foundInteger = true;
         ++iDTS;
      }
      if (foundInteger) {
         tokens.push (sum);
         if (sum < 100) {
            signature += "nn";
         } else {
            signature += "nnnn";
         }
      }
      return foundInteger;
   };
   
   function parse_month (mName, mValue) {
      if (dateTimeString.indexOf (mName) == iDTS) {
         tokens.push (mValue);
         signature += "m";
         iDTS += mName.length;
         return true;
      }
      return false;
   };

   function parse_month_text () {
      if (parse_month ("january", 1) || parse_month ("jan", 1) ||
          parse_month ("february", 2) || parse_month ("feb", 2) ||
          parse_month ("march", 3) || parse_month ("mar", 3) ||
          parse_month ("april", 4) || parse_month ("apr", 4) ||
          parse_month ("may", 5) || parse_month ("may", 5) ||
          parse_month ("june", 6) || parse_month ("jun", 6) ||
          parse_month ("july", 7) || parse_month ("jul", 7) ||
          parse_month ("august", 8) || parse_month ("aug", 8) ||
          parse_month ("september", 9) || parse_month ("sep", 9) ||
          parse_month ("october", 10) || parse_month ("oct", 10) ||
          parse_month ("november", 11) || parse_month ("nov", 11) ||
          parse_month ("december", 12) || parse_month ("dec", 12)) {
         return true;
      }
      return false;
   };

   function string_into_tokens () {
      while (iDTS < dateTimeString.length) {
         if (parse_integer ()) {
         } else if (parse_delimiter ()) {
         } else if (parse_month_text ()) {
         } else {
            return false;
         }
      }
      return true;
   };
}
