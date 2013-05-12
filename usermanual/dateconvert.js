/**
   regression test data (each date is just after midnight):
   jdn        gregorian  excel(win)  excel(mac)  gnumeric
   ---        ---------  ----------  ----------  --------
   2415020.5  1/1/1900   1           n/a         2
   2415077.5  2/27/1900  58          n/a         59
   2415078.5  2/28/1900  59          n/a         60
   n/a        2/29/1900  60          n/a         n/a
   2415079.5  3/1/1900   61          n/a         61
   2416480.5  1/1/1904   1462        0           1462
   2442413.5  1/1/1975   27395       25933       27395
   (note that excel for windows incorrectly considers 2/29/1900 as a valid date)
*/

// for writing to firefox console, set DEBUG to use dc_log ()
var DEBUG = false;
if (DEBUG) {
   netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
   var GBLTut_ConsoleService = Components.classes['@mozilla.org/consoleservice;1'].getService(Components.interfaces.nsIConsoleService);
}

var UNITS_WINDOWS = 0;
var UNITS_MACINTOSH = 1;
var UNITS_GNUMERIC = 2;

var UNITS_WINDOWS_TOKEN = "windows";
var UNITS_MACINTOSH_TOKEN = "macintosh";
var UNITS_GNUMERIC_TOKEN = "gnumeric";

var FORMAT_ENGLISH_TOKEN = "english";

function clear_form (frm) {
   frm.absNumIn1.value = "";
   frm.absDateIn1.value = "";
   frm.absDateOut1.value = "";
   frm.absNumIn2.value = "";
   frm.absDateIn2.value = "";
   frm.absDateOut2.value = "";
   frm.relNumIn1.value = "";
   frm.relDateOut1.value = "";
}
function convert_abs_string (objUnits, objFormat, objValue, objStringIn, objStringOut) {
   objValue.value = "";
   objStringOut.value = "";
   var d = new Date ();
   var valid = d.initialize (is_english (objFormat), objStringIn.value);
   if (valid) {
      var epoch = jdn_epoch (objUnits);
      objValue.value = gregorian_to_datevalue (objUnits, d, epoch);
      objStringOut.value = d.toString ();
   }
}
function convert_abs_value (objUnits, objValue, objStringIn, objStringOut) {
   objStringIn.value = "";
   objStringOut.value = "";
   if (is_abs_datevalue (objValue.value)) {
      var d = new Date ();
      var epoch = jdn_epoch (objUnits);
      var valid = datevalue_to_gregorian (objUnits, parseFloat (objValue.value), epoch, d);
      if (valid) {
         objStringOut.value = d.toString ();
      }
   }
}
function convert_rel_value (objValue, objStringOut) {
   var out = "";
   if (is_rel_datevalue (objValue.value)) {

      var value = parseFloat (objValue.value);
      var yr = Math.floor (value / 365.25); // average of 3 regular years at 365 days and 1 leap year
      var dmo = value - yr * 365.25;
      var mo = (dmo / (365.25 / 12.0)); // average of 12 months over one zero
      mo = Math.floor (mo * 10) / 10; // roundoff
      var ddy = dmo - mo * (365.25 / 12.0);
      var dy = Math.floor (value);
      var dhr = 24 * (value - dy);
      var hr = Math.floor (dhr);
      var dmn = 60 * (dhr - hr);
      var mn = Math.floor (dmn);
      var dsc = 60 * (dmn - mn);
      var sc = Math.floor (dsc);

      var delimiter = "";
      if ((dy > 0) || (dhr > 0)) { out += delimiter + dy + " days"; delimiter = ", "; }
      if ((hr > 0) || (dmn > 0)) { out += delimiter + hr + " hours"; delimiter = ", "; }
      if ((mn > 0) || (dsc > 0)) { out += delimiter + mn + " minutes"; delimiter = ", "; }
      if (sc > 0) { out += delimiter + sc + " seconds"; }
      if ((yr > 0) || (mo > 0)) {
         delimiter = "";
         out += " (about ";
         if (yr > 0) { out += yr + " year"; out += (yr > 1 ? "s" : ""); delimiter = " and "; }
         if (mo > 0) { out += delimiter + mo + " months"; delimiter = ", "; }
         out += ")";
      }
   }
   objStringOut.value = out;
}
function datevalue_to_gregorian (objUnits, value, epoch, date) {
   return jdn_to_gregorian (objUnits, epoch + value, date);
}
function dc_log (msg) {
   if (DEBUG) {
      netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
      GBLTut_ConsoleService.logStringMessage (msg);
   }
}
function gregorian_to_datevalue (objUnits, d, epoch) {
   var d_gregorian = gregorian_to_jdn (d);
   var datevalue = d_gregorian - epoch;

   if (windows_kludge (objUnits, d_gregorian)) {
      --datevalue;
   }

   return datevalue;
}
function gregorian_to_jdn (d) {
   var mo = d.getMonth () + 1;
   var a = Math.floor ((14.0 - mo) / 12.0);
   var y = d.getFullYear () + 4800 - a;
   var m = mo + 12 * a - 3;
   var total = d.getDate () + Math.floor ((153 * m + 2) / 5) + 365 * y +
      Math.floor (y / 4) - Math.floor (y / 100) + Math.floor (y / 400) -
      32045 - 0.5 + // half day since gregorian starts at noon
      universal_time (d.getHours (), d.getMinutes (), d.getSeconds ());

   return total;
}
function is_abs_datevalue (str) {
   var regexp = /^(\-)*\d+(\.\d*)?$/
   var result = str.match (regexp);
   return (null != result);
}
function is_english (objFormat) {
   for (i = 0; i < objFormat.length; i++) {
      if (objFormat[i].checked) {
         return (FORMAT_ENGLISH_TOKEN == objFormat[i].value);   
      }
   }
   return false;
}
function is_rel_datevalue (str) {
   var regexp = /^\d+(\.\d*)?$/
   var result = str.match (regexp);
   return (null != result);
}
function jdn_epoch (objUnits) {
   switch (units (objUnits)) {
      case UNITS_WINDOWS:
         // datevalue 0 is just after midnight on 12/31/1899 for first 60 days,
         // but we want datevalue 0 for 12/30/1899 for thousands of days after then
         return 2415018.5; 
      case UNITS_MACINTOSH:
         // datevalue 0 is just after midnight on 1/1/1904
         return 2416480.5;
      case UNITS_GNUMERIC:
         // datevalue 0 is just after midnight on 12/30/1899
         return 2415018.5;
   }
}
function jdn_to_gregorian (objUnits, jdn, date) {
   if (jdn < 0) {
      return false;
   }
   var j = jdn + 32044;
   var g = Math.floor (j / 146097);
   var dg = j % 146097;
   var c = Math.floor (((Math.floor (dg / 36524) + 1) * 3) / 4);
   var dc = dg - c * 36524;
   var b = Math.floor (dc / 1461);
   var db = dc % 1461;
   var a = Math.floor (((Math.floor (db / 365) + 1) * 3) / 4);
   var da = db - a * 365;
   da += 1;
   if (windows_kludge (objUnits, jdn)) {
      da += 1;
   }
   var y = g * 400 + c * 100 + b * 4 + a;
   var m = Math.floor ((da * 5 + 308) / 153) - 2;
   var d = da - Math.floor (((m + 4) * 153) / 5) + 122;
   var dd = d + 0.5;
   var yr = y - 4800 + Math.floor ((m + 2) / 12);
   var mo = (m + 2) % 12;
   var dy = Math.floor (dd);
   date.setYear (yr);
   date.setMonth (mo);
   date.setDate (Math.floor (dd));
   var dh = 24 * (dd % 1);
   var hr = Math.floor (dh);
   var dm = 60 * (dh - hr);
   var mn = Math.floor (dm);
   var ds = 60 * (dm - mn);
   date.setHours (hr);
   date.setMinutes (mn);
   date.setSeconds (ds);

   return true;
}
function test (objUnits) {
   var d0 = new Date ();
   d0.initialize (true, "1/1/1900");
   var jdn0 = gregorian_to_jdn (d0);
   alert ("jdn value of just after midnight 1/1/1900 was computed as "+jdn0+", and should be 2415020.5");
   var excel0 = gregorian_to_datevalue (d0, jdn0);
   alert ("excel value of 1/1/1900 was computed as "+excel0+", and should be 1");

   var d1 = new Date ();
   jdn_to_gregorian (objUnits, 2415019.5, d1);
   alert ("jdn value of 2415019.5 corresponds to gregorian date of "+d1.toString ()+", and should be just after midnight 12/31/1899")
      }
function units (objUnits) {
   for (i = 0; i < objUnits.length; i++) {
      if (objUnits[i].checked) {
         if (UNITS_WINDOWS_TOKEN == objUnits[i].value) { return UNITS_WINDOWS; }
         if (UNITS_MACINTOSH_TOKEN == objUnits[i].value) { return UNITS_MACINTOSH; }
         if (UNITS_GNUMERIC_TOKEN == objUnits[i].value) { return UNITS_GNUMERIC; }
      }
   }
   alert ("Unidentified date value units");

   return UNITS_WINDOWS;
}
function universal_time (hours, minutes, seconds) {
   return (hours + (minutes + seconds / 60) / 60) / 24;
}
function windows_kludge (objUnits, jdn) {
   if (UNITS_WINDOWS == units (objUnits)) {
      if (jdn < 2415079.5) { // julian date of just after midnight 3/1/1900
         // must fix excel for windows bug that includes imaginary date 2/29/1900
         return true;
      }
   }
   
   return false;
}
