%
% ot-doc.cls
% $Log$
% Revision 1.1  2006/12/06 20:29:46  andreas
% forgot some files at the last check in
%
% Revision 1.1  2005/01/18 19:51:19  georgs
% added kfi paper to doc/investigation/kfi-paper/
% sources are in doc/investigation/kfi/...
% furthermore i updated the week report
%
%


\NeedsTeXFormat{LaTeX2e}
\pdfoutput=1

\LoadClass[a4paper,11pt,pdftex,german,english,pointednumbers,
	headsepline]{scrartcl}

\RequirePackage[pdftex]{hyperref} % hyperlinks in the final PDF document
\RequirePackage[german,english]{varioref} % references in changing styles
\RequirePackage{xspace}   % spaces after macros
\RequirePackage{calc}     % to do length calculations
\RequirePackage[pdftex]{graphicx} % graphics package
\RequirePackage[pdftex]{color}    % color package
\RequirePackage[latin1]{inputenc} % encoding of the input file
\RequirePackage{afterpage} % to handle the float pages

\usepackage{listings}

% Acknowledgments
\newcommand{\acknowledgments}{%
\section*{\centering{Acknowledgments}}%
}

% Title and table of contents at start of document

\AtBeginDocument{
\maketitle % title
\pagestyle{headings}
\pagenumbering{roman} % table of contents with roman page numbers

\newpage
%\pagenumbering{arabic} % document with arabic page numbers

% Set parameters
\setlength{\parskip}{1.5 ex plus 1 ex minus 0.5 ex}
\setlength{\parindent}{0 em}
\sloppy

}
\AtEndDocument{
}
