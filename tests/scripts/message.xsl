<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" encoding="ISO-8859-1" />

<xsl:param name="mode">none</xsl:param>

<xsl:template match="/">
<result mode="{$mode}">
<xsl:choose>
 <xsl:when test="$mode='error'">Error<xsl:message terminate="yes">ERROR</xsl:message></xsl:when>
 <xsl:when test="$mode='warning'"><xsl:message terminate="no">WARNING</xsl:message></xsl:when>
 <xsl:otherwise><xsl:value-of select="$mode" /></xsl:otherwise>
</xsl:choose>
</result>
</xsl:template>

</xsl:stylesheet>