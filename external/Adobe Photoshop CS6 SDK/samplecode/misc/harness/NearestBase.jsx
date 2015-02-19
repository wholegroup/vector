// call a function so we can hide variables from other scripts
main();

function main() {

var timeIt = new Timer();

var errors = 0;

var eArray = new Array();

try {
// var gradientImportLog = new File( "~/Desktop/NearestBase.log" );
//gradientImportLog.open( "w", "TEXT", "????" );
// gradientImportLog.writeln( "width,\theight,\tmode,\tinvert,\ttransparent,\tImport Time" );

// Set the color picker to NearestBase
// this does not fail if the plug-in is missing
// but the other ones will ???
var idsetd = charIDToTypeID( "setd" );
    var desc39 = new ActionDescriptor();
    var idnull = charIDToTypeID( "null" );
        var ref4 = new ActionReference();
        var idPrpr = charIDToTypeID( "Prpr" );
        var idGnrP = charIDToTypeID( "GnrP" );
        ref4.putProperty( idPrpr, idGnrP );
        var idcapp = charIDToTypeID( "capp" );
        var idOrdn = charIDToTypeID( "Ordn" );
        var idTrgt = charIDToTypeID( "Trgt" );
        ref4.putEnumerated( idcapp, idOrdn, idTrgt );
    desc39.putReference( idnull, ref4 );
    var idT = charIDToTypeID( "T   " );
        var desc40 = new ActionDescriptor();
        var idClrr = charIDToTypeID( "Clrr" );
            var desc41 = new ActionDescriptor();
            var idPckr = charIDToTypeID( "Pckr" );
            var idPckK = charIDToTypeID( "PckK" );
            var idPlgP = charIDToTypeID( "PlgP" );
            desc41.putEnumerated( idPckr, idPckK, idPlgP );
            var idPckI = charIDToTypeID( "PckI" );
            desc41.putString( idPckI, """AdobeSDK NearestBase""" );
        var idClrk = charIDToTypeID( "Clrk" );
        desc40.putObject( idClrr, idClrk, desc41 );
        var idlegacyPathDrag = stringIDToTypeID( "legacyPathDrag" );
        desc40.putBoolean( idlegacyPathDrag, true );
    var idGnrP = charIDToTypeID( "GnrP" );
    desc39.putObject( idT, idGnrP, desc40 );
executeAction( idsetd, desc39, DialogModes.NO );

// =======================================================
var idsetd = charIDToTypeID( "setd" );
    var desc42 = new ActionDescriptor();
    var idnull = charIDToTypeID( "null" );
        var ref5 = new ActionReference();
        var idPrpr = charIDToTypeID( "Prpr" );
        var idCchP = charIDToTypeID( "CchP" );
        ref5.putProperty( idPrpr, idCchP );
        var idcapp = charIDToTypeID( "capp" );
        var idOrdn = charIDToTypeID( "Ordn" );
        var idTrgt = charIDToTypeID( "Trgt" );
        ref5.putEnumerated( idcapp, idOrdn, idTrgt );
    desc42.putReference( idnull, ref5 );
    var idT = charIDToTypeID( "T   " );
        var desc43 = new ActionDescriptor();
        var idMmrU = charIDToTypeID( "MmrU" );
        var idPrc = charIDToTypeID( "#Prc" );
        desc43.putUnitDouble( idMmrU, idPrc, 50.000000 );
    var idCchP = charIDToTypeID( "CchP" );
    desc42.putObject( idT, idCchP, desc43 );
executeAction( idsetd, desc42, DialogModes.NO );

// this is just setting colors
// you don't actually need the color picker installed!
// can i check the system info to find out if it is actually working
// make it a manual test?
// =======================================================
var idsetd = charIDToTypeID( "setd" );
    var desc44 = new ActionDescriptor();
    var idnull = charIDToTypeID( "null" );
        var ref6 = new ActionReference();
        var idClr = charIDToTypeID( "Clr " );
        var idFrgC = charIDToTypeID( "FrgC" );
        ref6.putProperty( idClr, idFrgC );
    desc44.putReference( idnull, ref6 );
    var idT = charIDToTypeID( "T   " );
        var desc45 = new ActionDescriptor();
        var idCyn = charIDToTypeID( "Cyn " );
        desc45.putDouble( idCyn, 0.000000 );
        var idMgnt = charIDToTypeID( "Mgnt" );
        desc45.putDouble( idMgnt, 100.000000 );
        var idYlw = charIDToTypeID( "Ylw " );
        desc45.putDouble( idYlw, 100.000000 );
        var idBlck = charIDToTypeID( "Blck" );
        desc45.putDouble( idBlck, 0.000000 );
    var idCMYC = charIDToTypeID( "CMYC" );
    desc44.putObject( idT, idCMYC, desc45 );
    var idSrce = charIDToTypeID( "Srce" );
    desc44.putString( idSrce, """photoshopPicker""" );
executeAction( idsetd, desc44, DialogModes.NO );

// =======================================================
var idsetd = charIDToTypeID( "setd" );
    var desc46 = new ActionDescriptor();
    var idnull = charIDToTypeID( "null" );
        var ref7 = new ActionReference();
        var idClr = charIDToTypeID( "Clr " );
        var idFrgC = charIDToTypeID( "FrgC" );
        ref7.putProperty( idClr, idFrgC );
    desc46.putReference( idnull, ref7 );
    var idT = charIDToTypeID( "T   " );
        var desc47 = new ActionDescriptor();
        var idCyn = charIDToTypeID( "Cyn " );
        desc47.putDouble( idCyn, 100.000000 );
        var idMgnt = charIDToTypeID( "Mgnt" );
        desc47.putDouble( idMgnt, 0.000000 );
        var idYlw = charIDToTypeID( "Ylw " );
        desc47.putDouble( idYlw, 100.000000 );
        var idBlck = charIDToTypeID( "Blck" );
        desc47.putDouble( idBlck, 0.000000 );
    var idCMYC = charIDToTypeID( "CMYC" );
    desc46.putObject( idT, idCMYC, desc47 );
    var idSrce = charIDToTypeID( "Srce" );
    desc46.putString( idSrce, """photoshopPicker""" );
executeAction( idsetd, desc46, DialogModes.NO );

// =======================================================
var idsetd = charIDToTypeID( "setd" );
    var desc50 = new ActionDescriptor();
    var idnull = charIDToTypeID( "null" );
        var ref9 = new ActionReference();
        var idClr = charIDToTypeID( "Clr " );
        var idFrgC = charIDToTypeID( "FrgC" );
        ref9.putProperty( idClr, idFrgC );
    desc50.putReference( idnull, ref9 );
    var idT = charIDToTypeID( "T   " );
        var desc51 = new ActionDescriptor();
        var idCyn = charIDToTypeID( "Cyn " );
        desc51.putDouble( idCyn, 100.000000 );
        var idMgnt = charIDToTypeID( "Mgnt" );
        desc51.putDouble( idMgnt, 100.000000 );
        var idYlw = charIDToTypeID( "Ylw " );
        desc51.putDouble( idYlw, 100.000000 );
        var idBlck = charIDToTypeID( "Blck" );
        desc51.putDouble( idBlck, 100.000000 );
    var idCMYC = charIDToTypeID( "CMYC" );
    desc50.putObject( idT, idCMYC, desc51 );
    var idSrce = charIDToTypeID( "Srce" );
    desc50.putString( idSrce, """photoshopPicker""" );
executeAction( idsetd, desc50, DialogModes.NO );

// =======================================================
var idsetd = charIDToTypeID( "setd" );
    var desc48 = new ActionDescriptor();
    var idnull = charIDToTypeID( "null" );
        var ref8 = new ActionReference();
        var idClr = charIDToTypeID( "Clr " );
        var idFrgC = charIDToTypeID( "FrgC" );
        ref8.putProperty( idClr, idFrgC );
    desc48.putReference( idnull, ref8 );
    var idT = charIDToTypeID( "T   " );
        var desc49 = new ActionDescriptor();
        var idCyn = charIDToTypeID( "Cyn " );
        desc49.putDouble( idCyn, 100.000000 );
        var idMgnt = charIDToTypeID( "Mgnt" );
        desc49.putDouble( idMgnt, 100.000000 );
        var idYlw = charIDToTypeID( "Ylw " );
        desc49.putDouble( idYlw, 0.000000 );
        var idBlck = charIDToTypeID( "Blck" );
        desc49.putDouble( idBlck, 0.000000 );
    var idCMYC = charIDToTypeID( "CMYC" );
    desc48.putObject( idT, idCMYC, desc49 );
    var idSrce = charIDToTypeID( "Srce" );
    desc48.putString( idSrce, """photoshopPicker""" );
executeAction( idsetd, desc48, DialogModes.NO );


	
}
catch(e) {
    alert(e + ":" + e.line);
	if ( e.message.search(/cancel/i) != -1 ) {
		w = maxWidth + 1;
		h = maxHeight + 1;
	}
	eArray[eArray.length] = e;
	errors++;
	// debugger;
} // end catch
//gradientImportLog.writeln( errors + " errors. " + tests + " tests in " + timeIt.getElapsed() + " seconds. " + tests/timeIt.getElapsed() + " tests/sec.");
//gradientImportLog.close();
//gradientImportLog.execute();

// end clean
//while (documents.length) {
//    activeDocument.close(SaveOptions.DONOTSAVECHANGES);
//}

// Reset the application preferences
// preferences.rulerUnits = startRulerUnits;
// preferences.typeUnits = startTypeUnits;
// displayDialogs = startDisplayDialogs;

//     1) " FAIL" for failures
//     2) " PASS" for test results OK
//     3) "  BUG" for known bugs, have the file name give the bug number
//     4) "ERROR" this comes from the harness if the script barfed/exception,
return errors == 0 ? ' PASS' : ' FAIL';

} // end function main

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// GradientImport()
// mode, "indX", "rgbC", "bitM", "gryS"
//////////////////////////////////////////////////////////////////////////////
function GradientImport( width, height, mode, invert, transparent ) {
    var idImpr = charIDToTypeID( "Impr" );
    var desc4 = new ActionDescriptor();
    var idUsng = charIDToTypeID( "Usng" );
    var desc5 = new ActionDescriptor();
    var idmulK = charIDToTypeID( "mulK" );
    var list2 = new ActionList();
    var desc6 = new ActionDescriptor();
    var idHrzn = charIDToTypeID( "Hrzn" );
    var idPxl = charIDToTypeID( "#Pxl" );
    desc6.putUnitDouble( idHrzn, idPxl, width );
    var idVrtc = charIDToTypeID( "Vrtc" );
    var idPxl = charIDToTypeID( "#Pxl" );
    desc6.putUnitDouble( idVrtc, idPxl, height );
    var idMd = charIDToTypeID( "Md  " );
    var idgrmT = charIDToTypeID( "grmT" );
    var idMode = charIDToTypeID( mode );
    desc6.putEnumerated( idMd, idgrmT, idMode );
    if ( invert ) {
        var idInvr = charIDToTypeID( "Invr" );
        desc6.putBoolean( idInvr, true );
    }
    if ( transparent ) {
        var idalpT = charIDToTypeID( "alpT" );
        desc6.putBoolean( idalpT, true );
    }
    var idmulS = charIDToTypeID( "mulS" );
    list2.putObject( idmulS, desc6 );
    desc5.putList( idmulK, list2 );
    var idgraD = charIDToTypeID( "graD" );
    desc4.putObject( idUsng, idgraD, desc5 );
    executeAction( idImpr, desc4, DialogModes.NO );
}

/****************************
  Function WaitForRedraw
  Usage: Use it to force Photoshop to redraw the screen before continuing
  Example: 
       WaitForRedraw();
****************************/
function WaitForRedraw() {
	var keyID = charIDToTypeID( "Stte" );
	var desc = new ActionDescriptor();
	desc.putEnumerated( keyID, keyID, charIDToTypeID( "RdCm" ) );
	executeAction( charIDToTypeID( "Wait" ), desc, DialogModes.NO );
}

//////////////////////////////////////////////////////////////////////////////
// WaitNSeconds, slow the script down so you can watch and figure out issues
//////////////////////////////////////////////////////////////////////////////
function WaitNSeconds(seconds) {
   startDate = new Date();
   endDate = new Date();
   while ((endDate.getTime() - startDate.getTime()) < (1000 * seconds)) 
		endDate = new Date();
}

//////////////////////////////////////////////////////////////////////////////
// FitOnScreen, fits the document and redraws the screen
//////////////////////////////////////////////////////////////////////////////
function FitOnScreen() {
	var id45 = charIDToTypeID( "slct" );
    var desc7 = new ActionDescriptor();
    var id46 = charIDToTypeID( "null" );
	var ref1 = new ActionReference();
	var id47 = charIDToTypeID( "Mn  " );
	var id48 = charIDToTypeID( "MnIt" );
	var id49 = charIDToTypeID( "FtOn" );
	ref1.putEnumerated( id47, id48, id49 );
	desc7.putReference( id46, ref1 );
	executeAction( id45, desc7, DialogModes.NO );
}

//////////////////////////////////////////////////////////////////////////////
// Library for timing things in JavaScript
//////////////////////////////////////////////////////////////////////////////
function Timer() {
	// member variables
	this.startTime = new Date();
	this.endTime = new Date();
	
	// member functions
	
	// reset the start time to now
	this.start = function () { this.startTime = new Date(); }
	
	// reset the end time to now
	this.stop = function () { this.endTime = new Date(); }
	
	// get the difference in milliseconds between start and stop
	this.getTime = function () { return (this.endTime.getTime() - this.startTime.getTime()) / 1000; }
	
	// get the current elapsed time from start to now, this sets the endTime
	this.getElapsed = function () { this.endTime = new Date(); return this.getTime(); }
}
// end NearestBase.jsx
