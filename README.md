## Build
To build Duilib or Demo, a user macro named "SOLUTION_ROOT" should be defined in Visual Studio like this:  
SOLUTION_ROOT = $([MSBuild]::GetDirectoryNameOfFileAbove($(ProjectDir), root)).

## A simple demo using this Duilib to render an video
![demo](https://raw.githubusercontent.com/ttyrion/duilib/master/demo.png) 
