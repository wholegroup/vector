// call a function so we can hide variables from other scripts
main();

function main() {

var errors = 0;

try {
        
    app.documents.add();

    // =======================================================
    var idpoorMans = stringIDToTypeID( "98b5a608-46ce-11d3-bd6b-0060b0a13dc4" );
    var desc65 = new ActionDescriptor();
    var idHrzn = charIDToTypeID( "Hrzn" );
    desc65.putInteger( idHrzn, 0 );
    var idVrtc = charIDToTypeID( "Vrtc" );
    desc65.putInteger( idVrtc, 0 );
    var idXFac = charIDToTypeID( "XFac" );
    desc65.putInteger( idXFac, 4 );
    var idGaBD = charIDToTypeID( "GaBD" );
    desc65.putBoolean( idGaBD, false );
    executeAction( idpoorMans, desc65, DialogModes.NO );
    
    var desc66 = new ActionDescriptor();
    var idHrzn = charIDToTypeID( "Hrzn" );
    desc66.putInteger( idHrzn, 50 );
    var idVrtc = charIDToTypeID( "Vrtc" );
    desc66.putInteger( idVrtc, 50 );
    var idXFac = charIDToTypeID( "XFac" );
    desc66.putInteger( idXFac, 16 );
    var idGaBD = charIDToTypeID( "GaBD" );
    desc66.putBoolean( idGaBD, false );
    executeAction( idpoorMans, desc66, DialogModes.NO );


    
    // activeDocument.close( SaveOptions.DONOTSAVECHANGES );
    
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

// end PoorMansTypeTool.jsx
