while IFS= read -r line; do
	xmllint --noblanks "$line" --output "$line"
done < <( find . -type f -name "*.ui" -print )
