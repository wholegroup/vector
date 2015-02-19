// call a function so we can hide variables from other scripts
main();

function main() {

var errors = 0;

try {
    
    // =======================================================
    var idAdobeSDKListener = stringIDToTypeID( "AdobeSDK Listener" );
    var desc28 = new ActionDescriptor();
    var idactS = charIDToTypeID( "actS" );
    desc28.putString( idactS, """Default Actions""" );
    var idactN = charIDToTypeID( "actN" );
    desc28.putString( idactN, """Vignette (selection)""" );
    executeAction( idAdobeSDKListener, desc28, DialogModes.NO );
    
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

// end Listener.jsx
