<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:h="http://www.w3.org/1999/xhtml" xmlns="http://www.w3.org/1999/xhtml" exclude-result-prefixes="h">
 <xsl:output method="html" doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"/>
 <xsl:template match="/h:html/h:head/h:title/text()">
  <xsl:text>404 Error | </xsl:text>
  <xsl:value-of select="substring-after(., ' | ')"></xsl:value-of>
 </xsl:template>
 <xsl:template match="//h:div[@class='header']/h:div[@class='headertitle']/h:div[@class='title']">
  <xsl:copy>
   <xsl:apply-templates select="@*"/>
   <xsl:text>404 Error</xsl:text>
  </xsl:copy>
 </xsl:template>
 <xsl:template match="//h:div[@class='header']/h:div[@class='subtitle']/text()">
  <xsl:text>Ah snap! We could not find what you are looking for!</xsl:text>
 </xsl:template>
 <xsl:template match="//h:div[@class='header']">
  <xsl:copy>
   <xsl:apply-templates select="@*|node()"/>
   <xsl:if test="not(h:div[@class='subtitle'])">
    <div class="subtitle">Ah snap! We could not find what you are looking for!</div>
   </xsl:if>
  </xsl:copy>
 </xsl:template>
 <xsl:template match="//h:ul[@class='tablist']/h:li[@class='current']/@class" />
 <xsl:template match="//h:div[@class='contents']/*">
  <p>Try to get back to the Home Page and start over!</p>
  <p><a href="{//h:div[@id='navrow1']/h:ul/h:li[1]/h:a/@href}">Go to the Home Page</a></p>
 </xsl:template>
 <xsl:template match="@*|node()">
  <xsl:copy>
   <xsl:apply-templates select="@*|node()"/>
  </xsl:copy>
 </xsl:template>
</xsl:stylesheet>
