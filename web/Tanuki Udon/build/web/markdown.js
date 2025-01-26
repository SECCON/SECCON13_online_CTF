const escapeHtml = (content) => {
  return content
    .replaceAll('&', '&amp;')
    .replaceAll(`"`, '&quot;')
    .replaceAll(`'`, '&#39;')
    .replaceAll('<', '&lt;')
    .replaceAll('>', '&gt;');
}

const markdown = (content) => {
  const escaped = escapeHtml(content);
  return escaped
    .replace(/!\[([^"]*?)\]\(([^"]*?)\)/g, `<img alt="$1" src="$2"></img>`)
    .replace(/\[(.*?)\]\(([^"]*?)\)/g, `<a href="$2">$1</a>`)
    .replace(/\*\*(.*?)\*\*/g, `<strong>$1</strong>`)
    .replace(/  $/mg, `<br>`);
}

module.exports = markdown;
