git remote set-branches --add origin master
git fetch
$CHANGED_FILES=$(git diff --name-only origin/master...${TRAVIS_COMMIT})
$ONLY_READMES=$true

$regex='(\.md)|(\.yml)|(\.sh)|(\.py)|(\.ps1)'

foreach ($CHANGED_FILE in $CHANGED_FILES) {
  if ( !($CHANGED_FILE -match $regex )) {
    $Global:ONLY_READMES=$false
    Write-Host "README STATE: $ONLY_READMES"
    break
  }
}

if ( $Global:ONLY_READMES -eq $true ) {
  Write-Host "Only non source code files found, exiting."
  $host.setshouldexit(1)
} else {
  Write-Host $(git diff --name-only origin/master...${TRAVIS_COMMIT})
  Write-Host "source code changes found, continuing with build."
}