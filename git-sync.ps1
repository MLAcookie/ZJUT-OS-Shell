param ( 
    [string]$commitMessage 
)
if (-not $commitMessage) {
    $commitMessage = (Get-Date).ToString("yyyy-MM-dd HH:mm:ss") 
}

git pull
git add .
git status
git commit -m "$commitMessage"
git push origin HEAD:main

Write-Output "Sync Complete"