/*
Copyright (c) 2003-2011, CKSource - Frederico Knabben. All rights reserved.
For licensing, see LICENSE.html or http://ckeditor.com/license
*/

CKEDITOR.editorConfig = function( config )
{
	// Define changes to default configuration here. For example:
	// 界面语言，默认为 'en'
    config.language = 'zh-cn';
    config.extraPlugins="syntaxhighlight";
	// 工具栏（基础'Basic'、全能'Full'、自定义）plugins/toolbar/plugin.js
   // config.toolbar = 'Full';   //config.toolbar = 'Full';  config.toolbar = 'Basic'; 
    
  //工具按钮.
    config.toolbar=
    [
    ['syntaxhighlight','Bold','Italic','Underline','Strike','-','Link','Unlink','Anchor','-','Subscript','Superscript','NumberedList','BulletedList','-','Outdent','Indent','Blockquote','JustifyLeft','JustifyCenter','JustifyRight','JustifyBlock'],
    ['Preview','Undo','Redo','RemoveFormat','Source'],
    '/',
    ['Styles','Font','FontSize'],['TextColor','BGColor'], ['Image','Flash','Table','HorizontalRule','Smiley','SpecialChar']
    ]; 
    
	//编辑器中回车产生的标签
    config.enterMode =CKEDITOR.ENTER_BR; //可选：CKEDITOR.ENTER_BR或CKEDITOR.ENTER_DIV、 CKEDITOR.ENTER_P
    //工具栏默认是否展开
    config.toolbarStartupExpanded = true;
 // 编辑器样式，有三种：'kama'（默认）、'office2003'、'v2'
    config.skin = 'v2';
 // 当提交包含有此编辑器的表单时，是否自动更新元素内的数据
    config.autoUpdateElement = true;
    //是否开启 图片和表格 的改变大小的功能 config.disableObjectResizing = true;
    config.disableObjectResizing = false //默认为开启
  //字体默认大小 plugins/font/plugin.js
    config.fontSize_defaultLabel = '14px';
  //载入时，以何种方式编辑 源码和所见即所得 "source"和"wysiwyg" plugins/editingblock/plugin.js.
    config.startupMode ='wysiwyg';
    
 // 取消 “拖拽以改变尺寸”功能 plugins/resize/plugin.js
    config.resize_enabled = true;
 // 设置宽高
  //  config.width = 980;
    config.height = 200;
    //改变大小的最大高度
   config.resize_maxHeight = 450;
    //改变大小的最大宽度
    config.resize_maxWidth = 720;
    //改变大小的最小高度
    config.resize_minHeight = 150;
    //改变大小的最小宽度
    config.resize_minWidth = 300;
    // 编辑器的z-index值
    config.baseFloatZIndex = 10000;
};
