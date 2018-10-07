[string[]]$Excludes = @('*external*', '*External*')

$allitems =get-childitem -recurse -include *.cpp,*.h,*.hpp | Where {$_.FullName -notlike "*\External\*"} 

foreach($file in $allitems)
{
    clang-format -verbose -style=file -i $file
}