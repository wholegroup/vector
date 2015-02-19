// call a function so we can hide variables from other scripts
main();

function main() {

var errors = 0;

try {
    
    app.documents.add();

    // =======================================================
    var idExpr = charIDToTypeID( "Expr" );
    var desc24 = new ActionDescriptor();
    var idUsng = charIDToTypeID( "Usng" );
    var desc25 = new ActionDescriptor();
    var idhisT = charIDToTypeID( "hisT" );
    desc24.putObject( idUsng, idhisT, desc25 );
    executeAction( idExpr, desc24, DialogModes.NO );
    
    activeDocument.close( SaveOptions.DONOTSAVECHANGES );
    
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

// end History.jsx
