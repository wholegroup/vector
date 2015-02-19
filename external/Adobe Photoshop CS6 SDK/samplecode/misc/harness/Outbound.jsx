// call a function so we can hide variables from other scripts
main();

function main() {
// Save the current preferences
var startRulerUnits = preferences.rulerUnits;
var startTypeUnits = preferences.typeUnits;
var startDisplayDialogs = displayDialogs;

// Set Photoshop to use pixels and display no dialogs
preferences.rulerUnits = Units.PIXELS;
preferences.typeUnits = TypeUnits.PIXELS;
displayDialogs = DialogModes.NO;

var errors = 0;

try {
    
    var f = new File( Folder.desktop + "/OutboundTest.exp" )

    f.remove();
    
    app.documents.add();

    // =======================================================
    var idExpr = charIDToTypeID( "Expr" );
    var desc62 = new ActionDescriptor();
    var idUsng = charIDToTypeID( "Usng" );
    var desc63 = new ActionDescriptor();
    var idIn = charIDToTypeID( "In  " );
    desc63.putPath( idIn, f );
    var idoutB = charIDToTypeID( "outB" );
    desc62.putObject( idUsng, idoutB, desc63 );
    executeAction( idExpr, desc62, DialogModes.NO );
    
    activeDocument.close( SaveOptions.DONOTSAVECHANGES );
    
    if ( ! f.exists )
        error++;
        
    if ( f.length < 1024 )
        error++;
    
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

// Reset the application preferences
preferences.rulerUnits = startRulerUnits;
preferences.typeUnits = startTypeUnits;
displayDialogs = startDisplayDialogs;

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

// end ColorMunger.jsx
