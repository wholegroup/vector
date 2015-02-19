app.documents.add();

// AutomationFilter, no params, fix up the plug-in
var idautomationFilter = stringIDToTypeID( "adc931a0-cfe2-11d5-98bf-00b0d0204936" );
executeAction( idautomationFilter, undefined, DialogModes.NO );
alert("You should see an AutomationFilter in the history panel.");

// MFCPlugIn, always pops dialog
var idFltr = charIDToTypeID( "Fltr" );
var desc35 = new ActionDescriptor();
var idUsng = charIDToTypeID( "Usng" );
desc35.putString( idUsng, """MFC PlugIn...""" );
executeAction( idFltr, desc35, DialogModes.NO );
alert("You should see an MFC plugIn in the history panel.");        

// TextAuto file format with TextAuto automation listener
var textLayerText = "Text Auto Test Data";
var f = new File( Folder.desktop + "/TextFormatTest.txt" );
if ( f.open('w') ) {
    f.writeln( textLayerText );
    f.close();
    alert("I created a text document on the Desktop. (TextFormatTest.txt) \r\
           Drag / Drop on Photoshop to open.\r\
           The background should be a text layer with.\r\
           'Text Auto Test Data' as the text.");
    app.system("explorer " + f.parent.fsName );
} else {
    alert("Could not create test document.");
}


