#!/usr/local/bin/php
<?
/*
    Copyright (C) 2002 KSVG Team
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

class ecma_class
{
  var $properties; // array of properties
  var $functions; // array of functions $functions[0][0]="geturl"; $functions[0][1]="2";
  var $constants; // array of functions
  var $class; // classname
  var $params;
  var $readonly;

  function ecma_class($name)
  {
    $this->class=$name;
    $this->functions=array();
    $this->properties=array();
    $this->constants=array();
    $this->parmas=array();
  }

  function add_content($content)
  {
    if ($content->type==0)
    {
      if (!in_array($content->name,$this->properties))
      {
        $this->properties[]=$content->name;
	$this->readonly[$content->name]=$content->readonly;
      }
    }
    elseif ($content->type==1)
    {
      if (!in_array($content->name,$this->functions))
      {
        $this->functions[]=$content->name;
	$this->params[$content->name]=$content->params;
      }
    }
    elseif ($content->type==2)
    {
      if (!in_array($content->name,$this->constants))
        $this->constants[]=$content->name;
    }
  }

  function to_text()
  {
    $headline=$this->class." (".(sizeof($this->properties))." Properties and ".(sizeof($this->functions))." Functions and ".(sizeof($this->constants))." Constants)";
    echo($headline."\n");
    echo("====================");
    echo("\n\n");

    
    echo("Properties\n");
    echo("----------\n");
    for ($i=0;$i<sizeof($this->properties);$i++)
    {
      echo($this->properties[$i]."\n");
    }

    echo("\n");
    echo("Functions\n");
    echo("----------\n");
    for ($i=0;$i<sizeof($this->functions);$i++)
    {
      echo($this->functions[$i]."\n");
    }
    
    echo("\n");
    echo("Constants\n");
    echo("----------\n");
    for ($i=0;$i<sizeof($this->constants);$i++)
    {
      echo($this->constants[$i]."\n");
    }

    echo("\n%%%%%%%%%%%%%%%%%%%%%%%%\n\n");
  }

  function to_html()
  {
    echo("<a name=\"".$this->class."\"></a>");
    echo("<a href=\"#toc\">Back to Index</a>");
    echo("<table border=\"1\" style=\"border-width: medium; border-collapse: collapse\">\n");
    echo("<tr><td colspan=\"3\"><h2 style=\"text-align: left\">".$this->class."</h2></td></tr>\n");
    
    if (sizeof($this->functions)>0)
    {
      echo("<tr><td colspan=\"3\"><h3>Functions</h3></td></tr>\n");

      for ($i=0;$i<sizeof($this->functions);$i++)
      {
	echo("<tr><td width=\"20\">&nbsp</td><td>".$this->functions[$i]."</td><td>".$this->params[$this->functions[$i]]." Parameter(s)</td></tr>\n");
      }
    }

    if (sizeof($this->properties)>0)
    {
      echo("<tr><td colspan=\"3\"><h3>Properties</h3></td></tr>\n");

      for ($i=0;$i<sizeof($this->properties);$i++)
      {
	if ($this->readonly[$this->properties[$i]])
	  $readonly="Property is ReadOnly";
	else
	  $readonly="Property is ReadWrite";
	  
	echo("<tr><td width=\"20\">&nbsp</td><td>".$this->properties[$i]."</td><td>$readonly</td></tr>\n");
      }
    }

    if (sizeof($this->constants)>0)
    {
      echo("<tr><td colspan=\"3\"><h3>Constants</h3></td></tr>\n");

      for ($i=0;$i<sizeof($this->constants);$i++)
      {
	echo("<tr><td width=\"20\">&nbsp</td><td>".$this->constants[$i]."</td></tr>\n");
      }
    }
    echo("</table><br><br>");
  }
  
  function to_svg(&$x,&$y)
  {
    $xtemp=$x+50;
    $ytemp=$y+50;
    
    echo("<text x=\"$xtemp\" y=\"$ytemp\">".$this->class."</text>");

    $ytemp+=10;
    
    if (sizeof($this->functions)>0)
    {
      echo("<text x=\"".$xtemp."\" y=\"".($ytemp)."\">Functions</text>\n");

      $ytemp+=10;
  
      for ($i=0;$i<sizeof($this->functions);$i++)
      {
	echo("<text x=\"".($xtemp+20)."\" y=\"".($ytemp)."\">".$this->functions[$i]." with ".$this->params[$this->functions[$i]]." Parameter(s)</text>\n");
	$ytemp+=10;
      }
    }

    if (sizeof($this->properties)>0)
    {
      echo("<text x=\"".$xtemp."\" y=\"".($ytemp)."\">Properties</text>\n");
      
      $ytemp+=10;
      
      for ($i=0;$i<sizeof($this->properties);$i++)
      {
	if ($this->readonly[$this->properties[$i]])
	  $readonly="Property is ReadOnly";
	else
	  $readonly="Property is ReadWrite";
	  
	echo("<text x=\"".($xtemp+20)."\" y=\"".$ytemp."\">".$this->properties[$i]." $readonly</text>\n");
	$ytemp+=10;
      }
    }

    if (sizeof($this->constants)>0)
    {
      echo("<text x=\"".$xtemp."\" y=\"".$ytemp."\">Constants</text>\n");

      $ytemp+=10;

      for ($i=0;$i<sizeof($this->constants);$i++)
      {
	echo("<text x=\"".($xtemp+20)."\" y=\"".$ytemp."\">".$this->constants[$i]."</text>\n");
	$ytemp+=10;
      }
    }
    $x=$xtemp+200;
  }
}

class kalyptus_result
{
  var $class;
  var $type; // 0 = Property ; 1 = Function ; 2 = Constant
  var $name;
  var $params;
  var $readonly;
  
  function kalyptus_result($class,$type,$name,$params=0)
  {
    $this->class=$class;
    $this->type=$type;
    $this->name=$name;
    $this->params=$params;
  }

  function setReadOnly($value)
  {
    $this->readonly=$value;
  }
}

function parse_kalyptus($line,&$fp,&$last_constructor)
{
  if (preg_match("/^[ \t^#]*([a-zA-Z_]*)[ \t]*KSVG::.*DontDelete/",$line,$match))
  {
    if (DEBUG) fputs($fp,"MATCH => Constant ".$match[1]." in class ".$match[2]."\n");
    return new kalyptus_result($last_constructor,2,$match[1]);
  }
  elseif (preg_match("/^[ \t^#]*([a-zA-Z_]*)[ \t]*([a-zA-Z_]*)::.*Function ([0-9])/",$line,$match))
  {
    if (DEBUG) fputs($fp,"MATCH => Function ".$match[1]." in class ".$match[2]."\n");
    return new kalyptus_result($match[2],1,$match[1],$match[3]);
  }
  elseif (preg_match("/^[ \t^#]*([a-zA-Z_]*)[ \t]*([a-zA-Z_]*)::.*DontDelete\|ReadOnly/",$line,$match))
  {
    if (DEBUG) fputs($fp,"MATCH => Property ".$match[1]." in class ".$match[2]."\n");
    $property=new kalyptus_result($match[2],0,$match[1]);
    $property->setReadOnly(true);
    return $property;
  }
  elseif (preg_match("/^[ \t^#]*([a-zA-Z_]*)[ \t]*([a-zA-Z_]*)::.*DontDelete/",$line,$match))
  {
    if (DEBUG) fputs($fp,"MATCH => Property ".$match[1]." in class ".$match[2]."\n");
    $property=new kalyptus_result($match[2],0,$match[1]);
    $property->setReadOnly(false);
    return $property;
  }
  elseif (preg_match("/^[ \t@begin^#]*([a-zA-Z_]*)Constructor::s_hashTable.*/",$line,$match))
  {
    if (DEBUG) fputs($fp,"Constructor => ".$match[1]."\n");
    $last_constructor=$match[1];
    return false;
  }
  else
  {
    if (DEBUG) fputs($fp,"Ignored => ".$line."\n");
    return false;
  }
}

function do_output($output,$ecma_classes)
{
  ksort($ecma_classes);

  reset($ecma_classes);

  switch($output)
  {
    case "txt":
      foreach ($ecma_classes as $classname => $obj)
      {
	$obj->to_text();
      }
      break;
    case "svg":
      echo("<svg width=\"100%\" height=\"100%\">\n");

      $x=0;
      $y=0;

      foreach ($ecma_classes as $classname => $obj)
      {
	$obj->to_svg($x,$y);
      }
      echo("</svg>");
      break;
    default:
      echo("<html>\n<body>\n");

      echo("<a name=\"toc\"><h1>Contents</h1></a><br>\n");

      foreach ($ecma_classes as $classname => $obj)
      {
	echo("<a href=\"#".$classname."\">$classname</a><br>\n");
      }

      echo("<hr>\n");

      foreach ($ecma_classes as $classname => $obj)
      {
	$obj->to_html();
      }
      echo("</body>\n</html>");
  }
}

function searchKalyptusCode($file,&$fp)
{
  global $ecma_classes;

  ob_start();
  readfile($file);
  $content=ob_get_contents();
  ob_end_clean();

  if (preg_match("/@begin(.*)@end/s",$content,$matches)) // FIXME broken....if several @end's are there it takes always the last
  {
    if (DEBUG) fputs($fp,"Found ".(sizeof($matches)-1)." Matches in ".$file."\n");
    for ($i=1;$i<sizeof($matches);$i++)
    {
      if (DEBUG) fputs($fp,"Checking ".$i.". Match\n");
      $lines=explode("\n",$matches[$i]);
      for ($j=0;$j<sizeof($lines);$j++)
      {
	$result=parse_kalyptus($lines[$j],$fp,$last_constructor);
	if ($result)
	  $results[]=$result;
      }
    }

    for ($i=0;$i<sizeof($results);$i++)
    {
      if ($ecma_classes[$results[$i]->class])
	$ecma_classes[$results[$i]->class]->add_content($results[$i]);
      else
      {
	$ecma_classes[$results[$i]->class]=new ecma_class($results[$i]->class);
	$ecma_classes[$results[$i]->class]->add_content($results[$i]);
      }
    }
  }
}

function crawlFiles($path)
{
  global $ecma_classes;

  $fp=fopen("php://stderr","w+");

  if ($dir = @opendir($path))
  {
    while (($file = readdir($dir)) !== false)
    {
      if (($file!=".") && ($file!=".."))
      {
	if (is_dir($path."/".$file))
	{
	  fputs($fp,"Entering directory ".$file."\n");
	  crawlFiles($path."/".$file);
	  fputs($fp,"Leaving directory ".$file."\n");
	}
	elseif (is_file($path."/".$file) && preg_match("/^[A-Za-z0-9_]+(\.cc|\.cpp|\.h|\.hpp)$/",$file))
	{
	  fputs($fp,"\tchecking $file\n");
	  searchKalyptusCode($path."/".$file,$fp);
	}
      }
    }	  
    closedir($dir);
  }
}

define(DEBUG,0);

$ecma_classes=array();

$ksvg_path="../";

$path=basename(realpath($ksvg_path));

if ($path!="ksvg")
{
  echo "Execute it in base ksvg dir please :S\n";
  return false;
}

crawlFiles($ksvg_path);

do_output($argv[1],$ecma_classes);
?>
