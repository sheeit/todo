#!/usr/bin/env perl

use 5.026;
use strict;
use warnings;

# Automatically die after failed system calls
use autodie;

# Use UTF-8 encoding
use utf8;

# Use croak/carp instead of die/warn
# (cf. Effective Perl Programming, item 102)
use Carp;

# Satisfy perlcritic
use version;
use Readonly;
Readonly our $VERSION => qv(0.0.1);

use English '-no_match_vars';

Readonly my $DEFAULT_FILE_NAME => join q(/), $ENV{HOME}, '.todo_dumpfile';

Readonly my $TASK_REGEX => qr { (?: [^\n] | [\n] [^\n] )+ }msox;

# TODO: This doesn't work for the first line, you should remove that stupid
# "Don't remove this line" thing, or at least add a ^L after it like every
# other item in the file so that we don't have to write special code for it.

Readonly my $LINE_REGEX =>
  qr{ \A \[ ( [x\ ] ) \] \t ( ${TASK_REGEX} ) \s* \z }msox;
Readonly my $INITIAL_NEWLINE => qr{ \A \n* }msox;

sub read_tasks {
    my ( $tasksref, $line ) = @_;
    if ( $line =~ $LINE_REGEX ) {
        push @{$tasksref}, { done => ( $1 eq 'x' ), task => $2 };
    }
    else {
        carp "Line [[[$line]]] doesn't match regex";
    }
    return;
}

if ( not @ARGV ) {
    carp "Using default dumpfile: $DEFAULT_FILE_NAME";
    push @ARGV, $DEFAULT_FILE_NAME;
}

my @tasks;

{
    local $INPUT_RECORD_SEPARATOR = qq(\f);    # ^L
    while (<>) {
        chomp;
        s/$INITIAL_NEWLINE//gmsox;
        &read_tasks( \@tasks, $_ );
    }
}

Readonly my @DONE_UNDONE => ( '[ ]', '[x]' );

my @counters;
my $i;
foreach my $task (@tasks) {
    printf "%d %s %s\n", $i++, $DONE_UNDONE[ $task->{done} ], $task->{task};
    ++$counters[ $task->{done} ];
}

printf "\n%d tasks total\n",       scalar @tasks;
printf "%d done     (%05.2f%%)\n", $counters[1], 100 * $counters[1] / @tasks;
printf "%d not done (%05.2f%%)\n", $counters[0], 100 * $counters[0] / @tasks;
