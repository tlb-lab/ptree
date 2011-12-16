Introduction
------------

**ptree** is a `PostgreSQL <http://www.postgresql.org/>`_ extension implementing
ptree, a variant of the `ltree <http://www.postgresql.org/docs/current/static/ltree.html>`_ 
data type. The ltree type is used for representing labels of data stored in a 
hierarchical tree-like structure. The ptree data type uses a forward slash '/' as 
a separator for the hierarchy and also allows the characters '`' and '-' inside 
labels.

Installation
------------

Software requirements
~~~~~~~~~~~~~~~~~~~~~
**ptree** requires PostgreSQL 9.1+ including development header files (normally 
postgresql-server-dev package or similar).

Obtaining the source code
~~~~~~~~~~~~~~~~~~~~~~~~~
The source code for **ptree** can be obtained by either downloading a source package
from the `Bitbucket <https://bitbucket.org/aschreyer/ptree>`_ repository or by cloning 
it with::

    $ hg clone https://bitbucket.org/aschreyer/ptree

Installing the extension
~~~~~~~~~~~~~~~~~~~~~~~~
The extension can be built and installed with::

    $ make
    $ sudo make install

This will build and install the shared library in the PostgreSQL server library
directory. The extension can then be installed in the server by executing the following
commands in PostgreSQL (e.g. psql or GUI tool)::

    $ CREATE EXTENSION ptree; 

License
-------
**ptree**, being a variant of the ltree contrib module, is released under the 
`PostgreSQL License <http://www.postgresql.org/about/licence/>`_.