git remote set-branches --add origin master
git fetch
$commit=$env:APPVEYOR_REPO_COMMIT
$branch=$env:APPVEYOR_REPO_BRANCH

$CHANGED_FILES=$(git diff --name-only origin/master...${commit})

if ($branch -eq "master") {
  $CHANGED_FILES=$(git diff --name-only HEAD^ HEAD) 
}
Write-Host "check against: $CHANGED_FILES"
Write-Host "branch is $branch"
$ONLY_READMES=$true

$regex='(\.md)|(\.yml)|(\.sh)|(\.py)|(\.ps1)'
$code='(\.cpp)|(\.h)'

foreach ($CHANGED_FILE in $CHANGED_FILES) {
  if ( !($CHANGED_FILE -match $regex )) {
    $script:ONLY_READMES=$false
  }
  if ($CHANGED_FILE -match $code) {
    $script:ONLY_READMES=$false;
  }
  break
}

if ( $Global:ONLY_READMES -eq $true ) {
  Write-Host "Only non source code files found, exiting."
  $host.setshouldexit(1)
} else {
  Write-Host $(git diff --name-only origin/master...${commit})
  Write-Host "source code changes found, continuing with build."
}