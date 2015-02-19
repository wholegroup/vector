// make this a function so we can hide these from harness script
main();

function main() {
var errors = 0;

var getterLog = new File( "~/Desktop/Getter.log" );

getterLog.remove();

// =======================================================
var idGetter = stringIDToTypeID( "459ac2e6-82d1-11d5-9879-00b0d0204936" );
executeAction( idGetter, undefined, DialogModes.NO );

if ( ! getterLog.exists ) 
    error++;
    
if ( getterLog.length < 100 )
    error++;
    
getterLog.execute();

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

// end Getter.jsx
