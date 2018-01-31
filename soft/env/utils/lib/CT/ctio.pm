package CT::ctio;

use strict;
use Term::ReadLine;

# Legacy perl (5.0*) on Sun does not know the 'our' operator
# Thus we do this the ugly way.
@CT::ctio::ISA = qw(Term::ReadLine);

sub new {
		my ($class, $title) = @_;
		my $self = $class->SUPER::new($title);
		return bless ($self, $class);
}

sub display {
		my $self = shift;
		my $OUT = $self->OUT;
		print $OUT "\n";
		local $\ = "\n";
		foreach (@_) {
				print $OUT $_;
		}
}

sub error {
		my $self = shift;
		$self->display(@_);
		exit 1;
}

sub confirmAbort {
		my $self = shift;
		$self->display(@_);
		my $answer = $self->readline("[default: n] > ");
		exit 1 if (not $answer or ($answer eq "n"));
}

sub askWithDefault {
		my ($self, $ask, $default_prompt, $default) = @_;
		my $m_prompt = "[default : $default_prompt] > ";
		my $OUT = $self->OUT;
		print $OUT "\n$ask\n";
		my $answer = $self->readline($m_prompt);
		return $default unless ($answer);
		return $answer;
}

sub askYesNo {
		my ($self, $ask, $default, $yes, $no) = @_;
		my $OUT = $self->OUT;
		my $m_prompt = "[default : $default] > ";
		print $OUT "\n$ask (y,n)\n";
		my $answer = $self->readline($m_prompt);
		$answer = $default unless ($answer);
		$answer =~ /^y/ and return $yes;
		$answer =~ /^n/ and return $no;
		return $answer;
}

sub readMultiline {
		my $self = shift;
		my @message = @_;
		push @message, "[<Return> for new line, <CTRL+D> (^D) to save and exit]";
		$self->display(@message);
		my @lines = ();
		while (defined ($_ = $self->readline(""))) {
				push @lines, $_;
		}
		return \@lines;
}

1;
