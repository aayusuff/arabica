<?xml version="1.0"?> 
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <!-- FileName: SORT01 -->
  <!-- Document: http://www.w3.org/TR/xslt -->
  <!-- DocVersion: 19991116 -->
  <!-- Section: 10 -->
  <!-- Purpose: Simple test for xsl:sort on numbers, default order. -->

<xsl:template match="doc">
  <out>
    Default (ascending) order....
    <xsl:for-each select="num">
      <xsl:sort data-type="number"/>
      <xsl:value-of select="."/><xsl:text> </xsl:text>
    </xsl:for-each>
  </out>
</xsl:template>

</xsl:stylesheet>
