/*Tips*/
.luluTip
{
    word-wrap:break-word; 
    position:absolute;
    width:150px;
    color: #a00;
    background-color:#FFFFCC;
    border:1px solid #a00;
    padding:2px;
    margin-top: 30px;
}
.luluTip div.triangle 
{
    background:transparent
        url('http://images.cnblogs.com/cnblogs_com/qleelulu/sj.gif')
        no-repeat scroll left top;
    position:absolute;
    height:17px;
    left:20px;
    top:-16px;
    width:31px;
    z-index:999;
}

/*code*/
.code {
background:#FBEDBB none repeat scroll 0 0;
border:1px solid #C0C0C0;
font-family:Verdana,Helvetica,"微软雅黑",Arial,"宋体",sans-serif;
margin:0 40px 0 20px;
padding:5px;
}
/*博客背景*/
body {
background-image:none;
background-color:#F4F8F9;
font-family:Verdana,Geneva,Arial,Helvetica,sans-serif;
font-size:13px;
line-height:150%;
margin:0px;
padding:0px;
}
/*博客顶部*/
.header {
background-image:none;
background-color:#C3D9FF;
border:medium none;
border-bottom:solid 1px #73880A;
height:60px!important;
height:80px;
padding-left:264px;
padding-top:30px;
}
a.headermaintitle:visited, a.headermaintitle:active, a.headermaintitle:link {
color:#36393D;
font-size:22px;
font-weight:bold;
text-decoration:none;
}
.headerText{
color:#3F4C6B;
}

/*博客左边*/
#leftcontentcontainer {
padding: 0px 10px 10px 10px;
}
#leftcontent {
background-color:#FFFFFF;
border-color:#C3D9FF;
border-style:dashed solid solid;
border-width:0px 1px 2px;
font-family:Arial;
font-size:12px;
left:20px;
position:absolute;
top:50px;
width:220px;
z-index:2;
}
/*顶部菜单*/
#mytopmenu {
background-color:White;
border:solid 1px #73880A;
border-style:none solid solid;
border-width:0px 1px 1px 1px;
font-size:12px;
margin-left:260px;
margin-right:20px;
margin-top:0px;
padding:5px 10px;
text-align:left;
}
#mylinks a {
color:#3F4C6B;
text-decoration:none;
}

/*主页分块样式*/
.day {
background-color:white;
border:1px solid #C3D9FF;
color:#73880A;
margin-bottom:20px;
padding:20px;
}
.dayTitle{
color:#008C00;
}
/*主文章样式*/
.post {
background-color:White;
border:1px solid #C3D9FF;
color:#4B4B4B;
font-size:13px;
padding:20px;
}
/*分类样式*/
H3 {
background-image:url();
background-repeat:no-repeat;
border-bottom:1px solid #008C00;
font-family:Verdana;
font-size:14.7px;
height:21px;
margin-bottom:10px;
}
.catListTitle{
background-image:url();
background-repeat:no-repeat;
border-bottom:1px solid #008C00;
}

/*评论的样式*/
.feedbackListSubtitle {style.css (line 358)
background-color:white;
border:1px solid #C3D9FF;
color:#8B8D72;
font-size:12px;
margin-bottom:8px;
padding:5px 7px 5px 5px;
}

/*显示一个警告文章要用的样式*/
.quick-alert {
   width: 50%;
   margin: 1em 0;
   padding: .5em;
   background: #ffa;
   border: 1px solid #a00;
   color: #a00;
   font-weight: bold;
   display: none;
 }

/*ToolTip样式定义*/
#luluTip
{
	word-break:break-all;
	position:absolute;
	width:150px;
	color: #a00;
	background-color:#FFFFCC;
	border:1px solid #a00;
	padding:10px;
	display:none;
}
#luluTip div.triangle
{
	background:transparent url('http://www.cnblogs.com/images/cnblogs_com/qleelulu/sj.gif') no-repeat scroll left top;
	height:17px;
	position:absolute;
	left:20px;
	top:-16px;
	width:31px;
	z-index:999;
}

/*浮动的信息*/
* { margin:0; padding:0; }
#FloatDiv
{
background:#FFFFCC url(http://www.cnblogs.com/images/cnblogs_com/qleelulu/CN.gif) no-repeat fixed 0% 50%;
position: fixed;
overflow: hidden;
left: 5px;
top: 5px;
Font-Size: 10pt;
Font-Family:Lucida Console;
width: 250px; 
height: 20px;
padding:2px;
color:#a00; 
border:1px solid #a00;
z-index:999;
}
#FloatDiv div{
margin: 1px 2px 1px 10px;
}
* html { background:url(null) no-repeat fixed; }
* html #FloatDiv { /* fix position for IE6 */
position: absolute;
top:expression(documentElement.scrollTop+5);
}

/*About me 开始*/
#AboutMe {
background: #C5E88E;
border-left: 1px solid #A1C665;
border-right: 1px solid #A1C665;
display:none;
padding: 5px;
}
.slide {
background: url(http://www.cnblogs.com/images/cnblogs_com/qleelulu/btn-slide.gif) no-repeat scroll center top;
border-top: 2px solid #A1C665;
height:  43px! important;
text-align: center;
margin: 0;
padding: 0;
}
.slideAboutMe {
	background: url(http://www.cnblogs.com/images/cnblogs_com/qleelulu/white-arrow.gif) no-repeat right -50px;
	text-align: center;
	width: 144px;
	height: 31px;
	padding: 10px 10px 0 0;
	margin: 0 auto;
	display: block;
	font: bold 120%/100% Arial, Helvetica, sans-serif;
	text-decoration: none;
}
.active {
	background-position: right 12px;
}
/*About me 结束*/

/*让现有的图片消失*/
#blogLogo{display:none;}
/*用超链接的背景图片呈现效果,可以根据你自己的图片设置width和height，以及用top和left来设置图片的位置*/
#lnkBlogLogo{display:block;width:121px;height:145px;top:5px;left:50px;background:url('http://www.cnblogs.com/images/cnblogs_com/qleelulu/blogSkinlogo.gif') no-repeat;}