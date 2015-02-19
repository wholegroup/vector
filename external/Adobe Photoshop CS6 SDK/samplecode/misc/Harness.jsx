var harnessVersion = "0.0.5";

// Run all the scripts found in a given location
// The individual scripts can be run one at a time for a visual result
// The individual scripts must conform to the following rules

//  A) Use a result variable to return:
//     1) " FAIL" for failures
//     2) " PASS" for test results OK
//     3) "  BUG" for known bugs, have the file name give the bug number
//     4) "ERROR" this comes from the harness if the script barfed/exception,
//         counts same as "FAIL"

//  B) Test for the harness global and show results to user accordingly
//     if (typeof gRunningInHarness == "undefined" ) {
//         alert(result);
//     }
//     result; // must be last

//  C) Leave Photoshop as you found it (soft requirement)
//     1) setting back preferences
//     2) closing documents you opened

var totalTime = new TimeIt();
var testTimes = 0;

var passCount = 0;
var bugCount = 0;
var failCount = 0;
var unknownCount = 0;

var gRunningInHarness = true;

// alert(Folder(File($.fileName).parent).toString());
var strLocation = Folder(File($.fileName).parent).toString(); // was here "~/JavaScripts/";
var strTestHarnessLocation = strLocation + "/harness/";
var strLogFileLocation = strTestHarnessLocation + "logfile/";
var strLogFileFullPath = strLogFileLocation + "LogFile.txt";

Folder( strLogFileLocation ).create();

var harnessLog = File( strLogFileFullPath );
harnessLog.open( "w" );

harnessLog.writeln( Date().toString() );
harnessLog.writeln( "Harness Version: " + harnessVersion );
harnessLog.writeln( "Application: " + app.name );
harnessLog.writeln( "Application Version: " + app.version );
harnessLog.writeln( "Application Build: " + app.build );
harnessLog.writeln( "Scripting Version: " + app.scriptingVersion );
harnessLog.writeln( "Scripting Build: " + app.scriptingBuildDate );
harnessLog.writeln( "ExtendScript Version: " + $.version );
harnessLog.writeln( "ExtendScript Build: " + $.build );
harnessLog.writeln( "ExtendScript Build Date: " + $.buildDate );
harnessLog.writeln();

var harnessFolder = Folder( strTestHarnessLocation );

var harnessFiles = harnessFolder.getFiles();

if ( harnessFiles.length == 0 ) {
	harnessLog.writeln( "ERROR: No files in harness" );
	failCount++;
}

for ( var harnessIndex = 0; harnessIndex < harnessFiles.length; harnessIndex++ ) {
	var timeIt = new TimeIt();
	try {
		if ( harnessFiles[ harnessIndex ] instanceof File ) {
			if ( harnessFiles[ harnessIndex ].open( "r" ) ) {
				var fileContents = harnessFiles[ harnessIndex ].read();
				harnessFiles[ harnessIndex ].close();
				var result = eval( fileContents );
				if ( result == " PASS" ) {
					passCount++;
				} else if ( typeof result == 'string' && 
                             result.search("BUG") != -1 ) {
					bugCount++;
				} else if ( result == " FAIL" ) {
					failCount++;
				} else {
					unknownCount++;
				}
                 timeIt.stop();
                 testTimes += timeIt.getTime();
				harnessLog.writeln( result + ": " + FormatString(timeIt.getTime()) + 
                                  " secs : " + harnessFiles[ harnessIndex ].name );
			} else {
				failCount++;
			}
		}
	}
	catch ( harnessError ) {
		// for ( aaa in harnessError ) alert( aaa +":"+harnessError[aaa]);
		harnessLog.writeln( "ERROR: " + FormatString(timeIt.getElapsed()) + " secs : " + 
                          harnessFiles[ harnessIndex ].name + " : " + harnessError + " : Line : " + 
                          harnessError.line + " : FileName : " + File( harnessError.fileName ).name );
		failCount++;
	}
}

totalTime.stop();

harnessLog.writeln();
harnessLog.writeln( "     PassCount: " + passCount );
harnessLog.writeln( "      BugCount: " + bugCount );
harnessLog.writeln( "     FailCount: " + failCount );
harnessLog.writeln( "  UnknownCount: " + unknownCount );
harnessLog.writeln( "        Pass %: " + FormatString(passCount / ( passCount + failCount ) * 100 ));
harnessLog.writeln( "    Total Time: " + FormatString(totalTime.getTime()) + " secs" );
harnessLog.writeln( "     Test Time: " + FormatString(testTimes) + " secs" );
harnessLog.writeln( "Harness Time %: " + FormatString((totalTime.getTime() - testTimes)/totalTime.getTime() * 100 ));
harnessLog.close();
harnessLog.execute();

function TimeIt() {
	// member variables
	this.startTime = new Date();
	this.endTime = new Date();
	
	// member functions
	
	// reset the start time to now
	this.start = function () { 
        this.startTime = new Date(); 
    }
	
	// reset the end time to now
	this.stop = function () { 
        this.endTime = new Date(); 
    }
	
	// get the difference in milliseconds between start and stop
	this.getTime = function () { 
        return (this.endTime.getTime() - this.startTime.getTime()) / 1000; 
    }
	
	// get the current elapsed time from start to now, this sets the endTime
	this.getElapsed = function () { 
        this.endTime = new Date(); return this.getTime(); 
    }
}


function FormatString(inNumber) {
    var strNumber = inNumber.toString();
    strNumber = strNumber.split(".");
    if (strNumber.length == 1)
        strNumber[1] = "000";
    while (strNumber[0].length < 3)
        strNumber[0] = " " + strNumber[0];
    if (strNumber[1] > 3)
        strNumber[1] = strNumber[1].substr(0, 3);
    while (strNumber[1].length < 3)
        strNumber[1] = strNumber[1] + "0";
    return strNumber.join(".");
}
// end Harness.jsx
