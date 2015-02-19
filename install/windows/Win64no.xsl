﻿<?xml version="1.0" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://schemas.microsoft.com/wix/2006/wi">
<xsl:output method="xml" version="1.0" />

	<xsl:template match="*[local-name()='Component']">
		<xsl:copy>
			<xsl:attribute name="Win64">no</xsl:attribute>
			<xsl:apply-templates select="@*|node()" />
		</xsl:copy>
	</xsl:template>

	<xsl:template match="@*|node()">
		<xsl:copy>
			<xsl:apply-templates select="@*|node()" /> 
		</xsl:copy>
	</xsl:template>

</xsl:stylesheet>