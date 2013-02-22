<h5>For use on external sites be sure to include the widget loader script:</h5>
<pre><code>

&lt;script type="text/javascript" src="http://- path to modules folder -/webform_goals/scripts/webform_goals.loader.js"&gt;&lt;script&gt;

</code></pre>

<h5>Standard template:</h5>
<p>Display the standard goal template</p>
<pre><code>

&lt;div class="wg-goal" data-gid="<?php print $goal_id; ?>"&gt;&lt;/div&gt;

</code></pre>

<h5>Custom template:</h5>
<p>Custom templates let you use goal tokens to build custom goal widget displays on the fly.</p>
<pre><code>
<?php print htmlentities('
<div class="wg-goal" data-gid="<?php print $goal_id; ?>">
  You may place custom text here. Goal data, including progress bar
  can be referenced here using the standard goal tokens:
  [webform_goal:name] - goal name
  [webform_goal:gid] - goal id
  [webform_goal:target] - target goal value
  [webform_goal:display_threshold] - goal progress required to display progress bar
  [webform_goal:start_date] - goal start date
  [webform_goal:end_date] - goal end date
  [webform_goal:progress-bar] - css progress bar
  [webform_goal:starter_message] - starter message
  [webform_goal:progress-starter] - progress bar, or starter message (uses display threshold
  [webform_goal:progress-raw] - total goal progress
  [webform_goal:progress-percent] - goal progress as a percentage of goal target
  [webform_goal:progress-percent-capped] - goal progress percent capped at 100 (useful for css)
  [webform_goal:progress-remaining-raw] - total goal unmet
  [webform_goal:progress-remaining-percent] - percentage of goal total unmet
  [webform_goal:progress-remaining-percent-capped] - percent of goal unmet, capped at 0 (useful for css)
  [webform_goal:progress-starter]
</div>
'); ?>
</code></pre>
